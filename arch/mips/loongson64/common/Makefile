# SPDX-License-Identifier: GPL-2.0
#
# Makefile for loongson based machines.
#

obj-y += setup.o init.o cmdline.o env.o time.o reset.o irq.o \
    bonito-irq.o mem.o machtype.o platform.o serial.o efi.o boardinfo.o \
    ioremap.o hypervisor.o

obj-$(CONFIG_PCI) += pci.o
obj-$(CONFIG_CPU_LOONGSON2) += dma.o

#
# Serial port support
#
obj-$(CONFIG_EARLY_PRINTK) += early_printk.o
obj-$(CONFIG_LOONGSON_UART_BASE) += uart_base.o
obj-$(CONFIG_LOONGSON_MC146818) += rtc.o

#
# Enable CS5536 Virtual Support Module(VSM) to virtulize the PCI configure
# space
#
obj-$(CONFIG_CS5536) += cs5536/

#
# Suspend Support
#

obj-$(CONFIG_SUSPEND) += pm.o
