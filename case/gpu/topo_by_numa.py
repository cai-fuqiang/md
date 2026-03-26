#!/usr/bin/env python3
import argparse
import json
import re
from dataclasses import dataclass, field
from typing import Dict, Iterable, List, Optional, Tuple

BRIDGE_NAMES = {"HostBridge", "PCIBridge"}

NUMA_RE = re.compile(r"NUMANode\s+L#(\d+)\s+\(P#(\d+)")
PACKAGE_RE = re.compile(r"Package\s+L#(\d+)")
PCI_RE = re.compile(r"PCI\s+([0-9a-fA-F]{2}:[0-9a-fA-F]{2}\.[0-7])\s+\(([^)]+)\)")
OFED_RE = re.compile(r'OpenFabrics\s+"([^"]+)"')
NET_RE = re.compile(r'Net\s+"([^"]+)"')
SPLIT_IB_NET_RE = re.compile(r".*v\d+$")


@dataclass
class Node:
    text: str
    indent: int
    children: List["Node"] = field(default_factory=list)


def parse_tree(lines: Iterable[str]) -> Node:
    root = Node(text="ROOT", indent=-1)
    stack: List[Node] = [root]
    for raw in lines:
        if not raw.strip():
            continue
        indent = len(raw) - len(raw.lstrip(" "))
        node = Node(text=raw.strip(), indent=indent)
        while stack and indent <= stack[-1].indent:
            stack.pop()
        stack[-1].children.append(node)
        stack.append(node)
    return root


def walk(node: Node) -> Iterable[Node]:
    for child in node.children:
        yield child
        yield from walk(child)


def parse_pci(node_text: str) -> Optional[Tuple[str, str]]:
    m = PCI_RE.match(node_text)
    if not m:
        return None
    return m.group(1), m.group(2)


def is_gpu_class(pci_class: str) -> bool:
    return pci_class.lower() in {"3d", "vga"}


def is_ib_class(pci_class: str) -> bool:
    return "infiniband" in pci_class.lower()


def find_numa_info(package_node: Node) -> Optional[Dict[str, str]]:
    for n in walk(package_node):
        m = NUMA_RE.search(n.text)
        if m:
            return {"logical": m.group(1), "physical": m.group(2)}
    return None


def unique_str_keep_order(items: Iterable[str]) -> List[str]:
    seen = set()
    out: List[str] = []
    for item in items:
        if item in seen:
            continue
        seen.add(item)
        out.append(item)
    return out


def unique_mlx_keep_order(items: Iterable[Dict[str, str]]) -> List[Dict[str, str]]:
    seen = set()
    out: List[Dict[str, str]] = []
    for item in items:
        key = (item.get("name", ""), item.get("bdf", ""))
        if key in seen:
            continue
        seen.add(key)
        out.append(item)
    return out


def is_split_ib_pci(pci_node: Node) -> bool:
    nets: List[str] = []
    for n in walk(pci_node):
        mm = NET_RE.search(n.text)
        if mm:
            nets.append(mm.group(1))
    return any(SPLIT_IB_NET_RE.match(net) for net in nets)


def extract_mlx_from_ib_pci(
    pci_node: Node,
    pci_bdf: str,
    ignore_split_ib: bool,
) -> List[Dict[str, str]]:
    if ignore_split_ib and is_split_ib_pci(pci_node):
        return []

    mlx: List[Dict[str, str]] = []
    for n in walk(pci_node):
        mm = OFED_RE.search(n.text)
        if mm:
            mlx.append({"name": mm.group(1), "bdf": pci_bdf})

    if not mlx:
        mlx.append({"name": pci_bdf, "bdf": pci_bdf})

    return unique_mlx_keep_order(mlx)


def analyze_bridge(
    bridge_node: Node,
    bridge_path: List[str],
    groups: List[Dict[str, object]],
    ignore_split_ib: bool,
) -> Dict[str, object]:
    current_path = bridge_path + [bridge_node.text]
    all_gpus: List[str] = []
    all_mlx: List[Dict[str, str]] = []
    child_has_both = False

    for child in bridge_node.children:
        if child.text in BRIDGE_NAMES:
            child_summary = analyze_bridge(child, current_path, groups, ignore_split_ib)
            all_gpus.extend(child_summary["gpus"])  # type: ignore[index]
            all_mlx.extend(child_summary["mlx"])  # type: ignore[index]
            if child_summary["has_both"]:  # type: ignore[index]
                child_has_both = True
            continue

        if not child.text.startswith("PCI "):
            continue

        pci_info = parse_pci(child.text)
        if not pci_info:
            continue
        pci_bdf, pci_class = pci_info

        if is_gpu_class(pci_class):
            all_gpus.append(pci_bdf)
        elif is_ib_class(pci_class):
            all_mlx.extend(extract_mlx_from_ib_pci(child, pci_bdf, ignore_split_ib))

    all_gpus = unique_str_keep_order(all_gpus)
    all_mlx = unique_mlx_keep_order(all_mlx)
    has_both = bool(all_gpus and all_mlx)

    if has_both and not child_has_both:
        groups.append(
            {
                "bridge": " > ".join(current_path),
                "gpus": all_gpus,
                "mlx": all_mlx,
            }
        )

    return {"gpus": all_gpus, "mlx": all_mlx, "has_both": has_both}


def build_report(root: Node, ignore_split_ib: bool) -> Dict[str, object]:
    result: Dict[str, List[Dict[str, object]]] = {}

    for n in walk(root):
        if not PACKAGE_RE.match(n.text):
            continue

        numa = find_numa_info(n)
        if not numa:
            continue

        groups: List[Dict[str, object]] = []
        for child in n.children:
            if child.text in BRIDGE_NAMES:
                analyze_bridge(child, [], groups, ignore_split_ib)

        result[f"numa{numa['physical']}"] = groups

    return result


def main() -> int:
    parser = argparse.ArgumentParser(description="按 NUMA 输出 bridge 下 GPU+IB(mlx) 分组 JSON")
    parser.add_argument("-i", "--input", default="lstopo.txt", help="lstopo 文本路径")
    parser.add_argument(
        "--ignore-split-ib",
        action="store_true",
        help="忽略IB切分卡（Net名形如 *v0/*v1/...）",
    )
    args = parser.parse_args()

    with open(args.input, "r", encoding="utf-8") as f:
        root = parse_tree(f.readlines())

    report = build_report(root, ignore_split_ib=args.ignore_split_ib)
    print(json.dumps(report, indent=2, ensure_ascii=False))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())#!/usr/bin/env python3
import argparse
import json
import re
from dataclasses import dataclass, field
from typing import Dict, Iterable, List, Optional, Tuple

BRIDGE_NAMES = {"HostBridge", "PCIBridge"}

NUMA_RE = re.compile(r"NUMANode\s+L#(\d+)\s+\(P#(\d+)")
PACKAGE_RE = re.compile(r"Package\s+L#(\d+)")
PCI_RE = re.compile(r"PCI\s+([0-9a-fA-F]{2}:[0-9a-fA-F]{2}\.[0-7])\s+\(([^)]+)\)")
OFED_RE = re.compile(r'OpenFabrics\s+"([^"]+)"')
NET_RE = re.compile(r'Net\s+"([^"]+)"')
SPLIT_IB_NET_RE = re.compile(r".*v\d+$")


@dataclass
class Node:
    text: str
    indent: int
    children: List["Node"] = field(default_factory=list)


def parse_tree(lines: Iterable[str]) -> Node:
    root = Node(text="ROOT", indent=-1)
    stack: List[Node] = [root]
    for raw in lines:
        if not raw.strip():
            continue
        indent = len(raw) - len(raw.lstrip(" "))
        node = Node(text=raw.strip(), indent=indent)
        while stack and indent <= stack[-1].indent:
            stack.pop()
        stack[-1].children.append(node)
        stack.append(node)
    return root


def walk(node: Node) -> Iterable[Node]:
    for child in node.children:
        yield child
        yield from walk(child)


def parse_pci(node_text: str) -> Optional[Tuple[str, str]]:
    m = PCI_RE.match(node_text)
    if not m:
        return None
    return m.group(1), m.group(2)


def is_gpu_class(pci_class: str) -> bool:
    return pci_class.lower() in {"3d", "vga"}


def is_ib_class(pci_class: str) -> bool:
    return "infiniband" in pci_class.lower()


def find_numa_info(package_node: Node) -> Optional[Dict[str, str]]:
    for n in walk(package_node):
        m = NUMA_RE.search(n.text)
        if m:
            return {"logical": m.group(1), "physical": m.group(2)}
    return None


def unique_str_keep_order(items: Iterable[str]) -> List[str]:
    seen = set()
    out: List[str] = []
    for item in items:
        if item in seen:
            continue
        seen.add(item)
        out.append(item)
    return out


def unique_mlx_keep_order(items: Iterable[Dict[str, str]]) -> List[Dict[str, str]]:
    seen = set()
    out: List[Dict[str, str]] = []
    for item in items:
        key = (item.get("name", ""), item.get("bdf", ""))
        if key in seen:
            continue
        seen.add(key)
        out.append(item)
    return out


def is_split_ib_pci(pci_node: Node) -> bool:
    nets: List[str] = []
    for n in walk(pci_node):
        mm = NET_RE.search(n.text)
        if mm:
            nets.append(mm.group(1))
    return any(SPLIT_IB_NET_RE.match(net) for net in nets)


def extract_mlx_from_ib_pci(
    pci_node: Node,
    pci_bdf: str,
    ignore_split_ib: bool,
) -> List[Dict[str, str]]:
    if ignore_split_ib and is_split_ib_pci(pci_node):
        return []

    mlx: List[Dict[str, str]] = []
    for n in walk(pci_node):
        mm = OFED_RE.search(n.text)
        if mm:
            mlx.append({"name": mm.group(1), "bdf": pci_bdf})

    if not mlx:
        mlx.append({"name": pci_bdf, "bdf": pci_bdf})

    return unique_mlx_keep_order(mlx)


def analyze_bridge(
    bridge_node: Node,
    bridge_path: List[str],
    groups: List[Dict[str, object]],
    ignore_split_ib: bool,
) -> Dict[str, object]:
    current_path = bridge_path + [bridge_node.text]
    all_gpus: List[str] = []
    all_mlx: List[Dict[str, str]] = []
    child_has_both = False

    for child in bridge_node.children:
        if child.text in BRIDGE_NAMES:
            child_summary = analyze_bridge(child, current_path, groups, ignore_split_ib)
            all_gpus.extend(child_summary["gpus"])  # type: ignore[index]
            all_mlx.extend(child_summary["mlx"])  # type: ignore[index]
            if child_summary["has_both"]:  # type: ignore[index]
                child_has_both = True
            continue

        if not child.text.startswith("PCI "):
            continue

        pci_info = parse_pci(child.text)
        if not pci_info:
            continue
        pci_bdf, pci_class = pci_info

        if is_gpu_class(pci_class):
            all_gpus.append(pci_bdf)
        elif is_ib_class(pci_class):
            all_mlx.extend(extract_mlx_from_ib_pci(child, pci_bdf, ignore_split_ib))

    all_gpus = unique_str_keep_order(all_gpus)
    all_mlx = unique_mlx_keep_order(all_mlx)
    has_both = bool(all_gpus and all_mlx)

    if has_both and not child_has_both:
        groups.append(
            {
                "bridge": " > ".join(current_path),
                "gpus": all_gpus,
                "mlx": all_mlx,
            }
        )

    return {"gpus": all_gpus, "mlx": all_mlx, "has_both": has_both}


def build_report(root: Node, ignore_split_ib: bool) -> Dict[str, object]:
    result: Dict[str, List[Dict[str, object]]] = {}

    for n in walk(root):
        if not PACKAGE_RE.match(n.text):
            continue

        numa = find_numa_info(n)
        if not numa:
            continue

        groups: List[Dict[str, object]] = []
        for child in n.children:
            if child.text in BRIDGE_NAMES:
                analyze_bridge(child, [], groups, ignore_split_ib)

        result[f"numa{numa['physical']}"] = groups

    return result


def main() -> int:
    parser = argparse.ArgumentParser(description="按 NUMA 输出 bridge 下 GPU+IB(mlx) 分组 JSON")
    parser.add_argument("-i", "--input", default="lstopo.txt", help="lstopo 文本路径")
    parser.add_argument(
        "--ignore-split-ib",
        action="store_true",
        help="忽略IB切分卡（Net名形如 *v0/*v1/...）",
    )
    args = parser.parse_args()

    with open(args.input, "r", encoding="utf-8") as f:
        root = parse_tree(f.readlines())

    report = build_report(root, ignore_split_ib=args.ignore_split_ib)
    print(json.dumps(report, indent=2, ensure_ascii=False))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())