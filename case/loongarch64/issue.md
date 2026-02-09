```
(gdb) p bus->maxSlot
$1 = 0
(gdb) p addr->slot
$2 = 1
(gdb) p bus
$3 = (virDomainPCIAddressBus *) 0x7fffd8058e90
(gdb) p bus->
flags                 isolationGroupLocked  minSlot               slot
isolationGroup        maxSlot               model
(gdb) p bus->model
$4 = VIR_DOMAIN_CONTROLLER_MODEL_PCIE_ROOT_PORT
(gdb) p bus->minSlot
$5 = 0
(gdb) p bus->maxSlot
$6 = 0
```
