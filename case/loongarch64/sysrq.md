## 现象
在龙芯环境终端上(kvm console), 执行sysrq 组合键没有反应:

<kbd>alt</kbd>+<kbd>PtrSc</kbd>+<kbd>c/m/t</kbd>

在 `Dell x86` 环境执行也是这样。并且通过BMC console小键盘输入也是这样。
我们来整体调试下

## sysrq调试

我们先从x86上调试

使用 `showkey -s`  获取按住

<kbd>alt</kbd>+<kbd>PtrSc</kbd>+<kbd>m</kbd>

在我的小键盘中为:
<kbd>alt</kbd>+<kbd>fn</kbd>+<kbd>Home</kbd>+<kbd>m</kbd>


`showkey -s ` 命令输出:
```
0x38 0x38 0xe0 0x47 0xe0 0xc7
0xae
0xe0 0xb8
```
我们来解释下:

| 扫描码序列 | 二进制分析 | 事件类型 | 对应按键与动作 |
| :--- | :--- | :--- | :--- |
| `0x38` (第1次) | `0011 1000` | 通码 (Make) | **左 Alt 键按下**。键码 `56`。 |
| `0x38` (第2次) | `0011 1000` | 通码 (Make) | **左 Alt 键自动重复**。按住不放产生的连续通码，表示 Alt 键一直处于压下状态。 |
| `0xe0 0x47` | `1110 0000`, `0100 0111` | 通码 (Make, 扩展) | **Home 键按下**。扩展键，由前缀 `0xe0` + 小键盘 `7` 键码 (`0x47`) 表示。 |
| `0xe0 0xc7` | `1110 0000`, `1100 0111` | 断码 (Break, 扩展) | **Home 键释放**。`0xe0` 前缀 + 小键盘 `7` 释放码 (`0x47 \| 0x80`)。 |
| `0xae` | `1010 1110` | 断码 (Break) | **C 键释放**。键码 `0x2e` (字母 `C`) + `0x80`。其按下动作（`0x2e`）发生在整段序列捕获之前。 |
| `0xe0 0xb8` | `1110 0000`, `1011 1000` | 断码 (Break, 扩展) | **右 Alt 键释放**。扩展键 `0xe0` + 左 Alt 键码的释放码 (`0x38 \| 0x80`)。同样，其按下动作 (`0xe0 0x38`) 发生在序列捕获之前。 |

看起来是有点乱的，<kbd>Fn</kbd>+<kbd>Home</kbd>, 似乎只输入了<kbd>Home</kbd>我们通过`bmc console` 小键盘来测试下:
```
0xb8 0x38 0x54 0xd4 0xb8
0xb2
```
解释如下:
| 扫描码序列 | 二进制分析 | 事件类型 | 对应按键与动作 |
| :--- | :--- | :--- | :--- |
| `0xb8` (第1次) | `1011 1000` | 断码 (Break) | **左 Alt 键释放**。`0x38 + 0x80`，说明序列开始前左 Alt 已处于按下状态。 |
| `0x38` | `0011 1000` | 通码 (Make) | **左 Alt 键再次按下**。为即将进行的组合键做准备。 |
| `0x54` | `0101 0100` | 通码 (Make) | **SysRq (系统请求) 键按下**。标准非扩展键，键码为 `0x54`。 |
| `0xd4` | `1101 0100` | 断码 (Break) | **SysRq 键释放**。`0x54 + 0x80`。 |
| `0xb8` (第2次) | `1011 1000` | 断码 (Break) | **左 Alt 键释放**。`0x38 + 0x80`，整个组合键操作完成。 |
| `0xb2` | `1011 0010` | 断码 (Break) | **M 键释放**。`0x32 + 0x80`（字母 M 的键码为 `0x32`）。其按下动作发生在序列捕获之前。 |


这次感觉正常了很多，但是<kbd>Alt</kbd>+<kbd>Sysrq(PtrSc)</kbd> 键位，在按<kbd>m</kbd>之前就已经松开。我们走读下代码，
看下这个行为符不符合预期:

## 代码走读

```cpp
static const struct input_device_id sysrq_ids[] = {
	{
		.flags = INPUT_DEVICE_ID_MATCH_EVBIT |
				INPUT_DEVICE_ID_MATCH_KEYBIT,
		.evbit = { [BIT_WORD(EV_KEY)] = BIT_MASK(EV_KEY) },
		.keybit = { [BIT_WORD(KEY_LEFTALT)] = BIT_MASK(KEY_LEFTALT) },
	},
	{ },
};
static struct input_handler sysrq_handler = {
	.filter		= sysrq_filter,
	.connect	= sysrq_connect,
	.disconnect	= sysrq_disconnect,
	.name		= "sysrq",
	.id_table	= sysrq_ids,
};
```

每个输入设备(struct input_dev)内部都维护若干位图(bitmap),用来声明"我支持哪些能力",典型的有:

* `evbit[]`:支持哪些事件类型,如 `EV_KEY`(按键)、`EV_REL`(相对坐标)、`EV_ABS`(绝对坐标)……
* `keybit[]`:具体支持哪些键码,如 `KEY_A`、`KEY_LEFTALT` 等
* `relbit[]、absbit[]、ledbit[]、swbit[]` …… 一一对应不同类别

事件具体类型如下:


| 事件类型 | 值 | 含义 | 典型 code |
|---|---|---|---|
| EV_SYN | 0x00 | 同步事件,标记一组事件结束 | SYN_REPORT |
| EV_KEY | 0x01 | 按键/按钮(键盘键、鼠标键、手柄键) | KEY_A, KEY_LEFTALT, BTN_LEFT |
| EV_REL | 0x02 | 相对位移(鼠标移动) | REL_X, REL_Y, REL_WHEEL |
| EV_ABS | 0x03 | 绝对坐标(触摸屏、平板) | ABS_X, ABS_MT_POSITION_X |
| EV_MSC | 0x04 | 杂项(scancode 等) | MSC_SCAN |
| EV_SW | 0x05 | 开关(盖板开合、耳机插拔) | SW_LID |
| EV_LED | 0x11 | LED 状态(大写锁、数字锁指示灯) | LED_CAPSL |
| EV_SND | 0x12 | 声音(蜂鸣) | SND_BELL |
| EV_REP | 0x14 | 按键重复速率配置 | REP_DELAY |
| EV_FF | 0x15 | 力反馈 | FF_RUMBLE |


所以`sysrq_ids` 表示`input_device` 需要满足

`struct input_handler`(如 SysRq、evdev、keyboard 等)通过 `id_table`(一个 struct input_device_id 数组)声明"我要处理哪类设备"。但 `input_device_id` 里字段很多,默认情况下全部置零会被当成"无所谓"——所以必须用 flags 显式指明:"这些字段我要参与匹配"。

两个标志的含义
* INPUT_DEVICE_ID_MATCH_EVBIT(0x0010):  表示"请比对 evbit 字段"。
* INPUT_DEVICE_ID_MATCH_KEYBIT(0x0020): 表示"请比对 keybit 字段"。

而该代码中, 只置了 `KEY_LEFTALT`,于是要求:设备必须能上报左 Alt 键——这基本就只有键盘类设备满足。
两者用 | 组合后写入 flags,意味着两个条件必须同时满足(AND 语义):既要支持 EV_KEY 事件,又要支持 KEY_LEFTALT 键。

真正执行比对的函数是:`input_match_device_id()`  ,它会先检查 id->flags 里每一位,
如果置了 MATCH_EVBIT 就用类似下面的逻辑验证 evbit,否则跳过该字段;
MATCH_KEYBIT 同理,然后再检查 keybit。

```
if (id->flags & INPUT_DEVICE_ID_MATCH_EVBIT)
    if (!bitmap_subset(id->evbit, dev->evbit, EV_MAX))
        return false;

if (id->flags & INPUT_DEVICE_ID_MATCH_KEYBIT)
    if (!bitmap_subset(id->keybit, dev->keybit, KEY_MAX))
        return false;
```

在同一段定义里还有:

`MATCH_BUS / VENDOR / PRODUCT / VERSION(0x01~0x08)`:按总线号、厂商、产品、版本号精确匹
配(类似 USB/PCI 设备 ID) `MATCH_RELBIT / ABSBIT / LEDBIT / SWBIT / PROPBIT` 等
按相对轴、绝对轴、LED、开关、设备属性等能力位图匹配 它们的使用方式与 `EVBIT / KEYBIT` 
完全一致——flag 开关 + 对应位图字段,组合出灵活的设备筛 选条件。

接下来我们主要看 `.filter`代码:
```
static bool sysrq_filter(struct input_handle *handle,
			 unsigned int type, unsigned int code, int value)
{
	struct sysrq_state *sysrq = handle->private;
	bool suppress;

	/*
	 * Do not filter anything if we are in the process of re-injecting
	 * Alt+SysRq combination.
	 */
	if (sysrq->reinjecting)
		return false;

	switch (type) {

	case EV_SYN:
		suppress = false;
		break;

	case EV_KEY:
		suppress = sysrq_handle_keypress(sysrq, code, value);
		break;

	default:
		suppress = sysrq->active;
		break;
	}

	return suppress;
}
```

* return value:
  + 首先`.filter` 的返回值表示 是否向用户态传递该事件, 如果接收到`EV_KEY`事件，并且
    `sysrq_handle_keypress()` 返回true，则不将该事件传递到用户态，认为该事件是内核
    在处理`sysrq`, 被内核给"吞"了
* `EV_SYN`:
  + `EV_SYN` 是比较特殊的, 是 input 子系统的同步事件类型,值为 0x00。它本身不携带任何"内容"意义,
    而是作为帧/包的分隔符使用:设备一次物理动作可能会产生多个原子事件(例如触摸屏一次触摸同
    时上报 ABS_X、ABS_Y、BTN_TOUCH),驱动在这些事件的末尾发送一个 EV_SYN/SYN_REPORT,告诉消
    费者"这一组事件到此为止,可以整体处理了"。

    而 `sysrq` 不对此事件做任何拦截, `default` 分支表示对其他事件做拦截.

好接下来我们分析`sysrq_handle_keypress()`

```cpp
static bool sysrq_handle_keypress(struct sysrq_state *sysrq,
				  unsigned int code, int value)
{
	bool was_active = sysrq->active;
	bool suppress;

	switch (code) {

	case KEY_LEFTALT:
	case KEY_RIGHTALT:
		if (!value) {
			/* One of ALTs is being released */
			if (sysrq->active && code == sysrq->alt_use)
				sysrq->active = false;

			sysrq->alt = KEY_RESERVED;

		} else if (value != 2) {
			sysrq->alt = code;
			sysrq->need_reinject = false;
		}
		break;

	case KEY_LEFTSHIFT:
	case KEY_RIGHTSHIFT:
		if (!value)
			sysrq->shift = KEY_RESERVED;
		else if (value != 2)
			sysrq->shift = code;
		if (sysrq->active)
			sysrq->shift_use = sysrq->shift;
		break;

	case KEY_SYSRQ:
		if (value == 1 && sysrq->alt != KEY_RESERVED) {
			sysrq->active = true;
			sysrq->alt_use = sysrq->alt;
			/* either RESERVED (for released) or actual code */
			sysrq->shift_use = sysrq->shift;
			/*
			 * If nothing else will be pressed we'll need
			 * to re-inject Alt-SysRq keysroke.
			 */
			sysrq->need_reinject = true;
		}

		/*
		 * Pretend that sysrq was never pressed at all. This
		 * is needed to properly handle KGDB which will try
		 * to release all keys after exiting debugger. If we
		 * do not clear key bit it KGDB will end up sending
		 * release events for Alt and SysRq, potentially
		 * triggering print screen function.
		 */
		if (sysrq->active)
			clear_bit(KEY_SYSRQ, sysrq->handle.dev->key);

		break;

	default:
		if (sysrq->active && value && value != 2) {
			unsigned char c = sysrq_xlate[code];

			sysrq->need_reinject = false;
			if (sysrq->shift_use != KEY_RESERVED)
				c = toupper(c);
			__handle_sysrq(c, true);
		}
		break;
	}

```
该代码较长，我们分几个部分解释，首先该部分囊扩了对不同键位的处理, `code` 表示该事件的具体对象,
在`EV_KEY`事件中，表示具体的键位, 而`value` 表示该对象的状态或者数值, 其中`EV_KEY`事件对象value
如下表示:
* 0 : 松开
* 1 : 按下
* 2 : 自动重复

我们再来说 `sysrq_state` 成员:
* **active** : 表示<kbd>alt</kbd> + <kbd>sysrq</kbd> 键位已经按下，等待下一个键位<kbd>c/m/t</kbd> 触发
  特定的sysrq功能
* `alt, alt_use`: 按/松 alt 键位时，赋值这两个值:
  * **alt** : 表示 最后一次摁alt的具体的code.
  * **alt_use**: 表示摁 <kbd>alt</kbd>+<kbd>sysrq</kbd> 时, alt的值

  > [!NOTE]
  > 
  > 为什么搞这么复杂呢，因为<kbd>Alt</kbd>分为<kbd>Left Alt</kbd>和<kbd>Right Alt</kbd>,
  > 我们假设一个场景，依次触发下面事件
  > + 按 <kbd>Left Alt</kbd>
  > + 按 <kbd>Right Alt</kbd>
  > + 按 <kbd>Sysrq</kbd>
  > + 松 <kbd>Left Alt</kbd>
  > + 按 <kbd>m</kbd>
  >
  > 这时，我们要不要触发`sysrq`, 按照现在的流程是要的。
  >
  > 但是如果是下面的情况:
  >
  > + 按 <kbd>Left Alt</kbd>
  > + 按 <kbd>Right Alt</kbd>
  > + 松 <kbd>Left Alt</kbd>
  > + 按 <kbd>Sysrq</kbd>
  > + 按 <kbd>m</kbd>
  > 
  > 和
  > + 按 <kbd>Left Alt</kbd>
  > + 按 <kbd>Right Alt</kbd>
  > + 按 <kbd>Sysrq</kbd>
  > + 松 <kbd>Right Alt</kbd>
  > + 按 <kbd>m</kbd>
  >
  > 这种情况就触发不了sysrq, 总之`kernel` 为了应对左右alt, 指定了一些规则，规定只有特殊的按键情况下，
  > 才会触发sysrq, 当然我们如果只操作一个alt，那肯定会触发
* `shift, shift_use`: sysrq触发不是 <kbd>Alt</kbd>+<kbd>sysrq</kbd>+<kbd>c/m/t</kbd>么, 为什么还会有
  `shift`的事情, `shift` 按键和我们平时按`shift` 的作用是一样的，就是为了临时切换大小写。按了
  <kbd>shift</kbd>后，我们如果再次按<kbd>m</kbd>,  就相当于按了<kbd>M</kbd>

  `toupper()` 函数起到了将小写字符变大写。

* **need_reinject**: `reinject`的意思是 "再次注入", 在这里表示当`sysrq`事件消失时，需要将之前已经拦截
  的键位，在全部下发到用户态 ("再次注入") 大概流程如下所示:

  ```
  用户按 Alt+SysRq
          │
          ├─ 吞掉 Alt、SysRq 事件(filter suppress)
          ├─ active = true
          └─ need_reinject = true   ← 乐观标记
                  │
      ┌───────────┼──────────────────┐
      │                              │
  用户按 b/s/e…(使用 SysRq)     用户松开 Alt(没按任何字母)
      │                              │
      ├─ 查表执行 __handle_sysrq     ├─ active = false
      ├─ need_reinject = false        ├─ schedule_work(reinject_work)
      │   ↓                          │       ↓
  └── 退出 SysRq 时:                  └── 若 need_reinject == true
      work 被调度,但 need_reinject==false,         → 伪造 Alt+SysRq 完整按键序列注入
      什么也不做                                    → 下游应用依旧能收到这次键,不会丢键
  ```

OK, 上面的流程就简单描述到这里。我们来看下面的流程。

```
	suppress = sysrq->active;

	if (!sysrq->active) {

		/*
		 * See if reset sequence has changed since the last time.
		 */
		if (sysrq->reset_seq_version != sysrq_reset_seq_version)
			sysrq_parse_reset_sequence(sysrq);

		/*
		 * If we are not suppressing key presses keep track of
		 * keyboard state so we can release keys that have been
		 * pressed before entering SysRq mode.
		 */
		if (value)
			set_bit(code, sysrq->key_down);
		else
			clear_bit(code, sysrq->key_down);

		if (was_active)
			schedule_work(&sysrq->reinject_work);

		/* Check for reset sequence */
		sysrq_detect_reset_sequence(sysrq, code, value);

	} else if (value == 0 && test_and_clear_bit(code, sysrq->key_down)) {
		/*
		 * Pass on release events for keys that was pressed before
		 * entering SysRq mode.
		 */
		suppress = false;
	}

	return suppress;
}
```

下面的流程也比较简单

我们先看`sysrq->key_down`相关逻辑:
* `!sysrq->active`
  + 如果`sysrq->active`为假，根据前一行代码, `suppress` 返回值也为假。假设在sysrq使能之前摁住按键一定
    是放行的
* `sysrq->active`
  + 说明此时 已经触发了`sysrq`, 而之前按住的键（都保存到 `key_down`) 松开，则正常放行

我们再来看`reinject_work`: 如果`was_active` 为真，说明本次事件执行之前已经按了
<kbd>alt</kbd>+<kbd>sysrq</kbd>, 而本次操作导致`active` 为假了. 当`value == 0`时，并且
该按键在`key_down` 中，则说明在sysrq active之前，该按键被按住，则需要放行。

最后，我们简单描述下`sysrq reset sequence`:

SysRq 除了 <kbd>Alt</kbd>+<kbd>SysRq</kbd>+<kbd>XXX</kbd> 这种软件触发外,还支持一个独立的 
**按键复位序列** 机制(常用于嵌入式/平板/无 SysRq 键的设备):

管理员通过 `sysfs(/sys/module/sysrq/parameters/reset_seq)`或设备树节点 `/chosen/linux,sysrq-reset-seq`(见 
`sysrq_of_get_keyreset_config()`)配置一组键,比如 `KEY_LEFTALT + KEY_LEFTSHIFT + KEY_POWER`。
当用户同时按住这组按键(且保持一段 reset_downtime_ms 时间)时,内核会走 `sysrq_handle_reset_request()`
执行有序重启 `orderly_reboot()`, 相当于一个硬件级的"强制重启组合键"。
检测逻辑由 `sysrq_detect_reset_sequence()`负责,它需要知道"当前这组要监视的键是哪些"——这份信息就被
预计算在 `sysrq->reset_keybit` 位图里,由 `sysrq_parse_reset_sequence()`生成。

Ok, 流程基本解释完了，我们接下来进一步调试 `sysrq_filter()` 的具体流程:


## 继续调试

使用下面bpftrace脚本进行调试:

[EBPF](./ebpf/sysrq/sysrq.bpf)

在终端按键<kbd>alt</kbd>+<kbd>fn</kbd>+<kbd>sysrq</kbd>+<kbd>m</kbd>

输出如下:

[bpftrace print keyboard](./ebpf/sysrq/keyboard_alt_fn_Home_m.txt)

可以发现按键已经全部乱套了。(甚至出现了<kbd>F13</kbd>)


我们再使用 BMC 终端上的小键盘, 输出如下 :

[bpftrace print keyboard](./ebpf/sysrq/mini_keyboard_alt_fn_Home_m.txt)

可以看到问题点在于在按 <kbd>m</kbd> 之前, <kbd>Left alt</kbd> 被松开了。这个非常奇怪。

所以，我们需要才用另一种方式来解决这个问题。

## 解决方法

我们需要解决如下问题:
* 避免alt 被松开
* 替换`PrtSc`为其他按键

为此，我们做:

* 如果检测到 Alt键位被松开后，我们延迟该松开动作
* 将 `F12` 键位在sysrq 处理中等效为`PrtSc`键位

我们使用 `kprobe` 完成这个功能

最终程序

[sysrq_alt_f12](./sysrq_alt_f12/)

**效果**:

执行 <kbd>Right Alt</kbd>+<kbd>F12</kbd>+<kbd>xxx</kbd>可以触发
sysrq功能
