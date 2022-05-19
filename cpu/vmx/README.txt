# A.6 MISCELLANEOUS DATA
## IA32_VMX_MISC
Bits 4:0 report a value X that specifies the relationship 
between the rate of the VMX-preemption timer and thatof 
the timestamp counter (TSC). Specifically, the VMX-preemption 
timer (if it is active) counts down by 1 everytime bit X in 
the TSC changes due to a TSC increment.
