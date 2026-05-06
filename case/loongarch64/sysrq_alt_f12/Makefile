# SPDX-License-Identifier: GPL-2.0
KVER ?= $(shell uname -r)
KDIR ?= /usr/src/kernels/$(KVER)

obj-m += sysrq_alt_f12_kprobe.o

all:
	$(MAKE) -C $(KDIR) M=$(CURDIR) modules

clean:
	$(MAKE) -C $(KDIR) M=$(CURDIR) clean
