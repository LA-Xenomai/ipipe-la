# SPDX-License-Identifier: GPL-2.0
# Most of this file is copied from tools/lib/traceevent/Makefile

BPF_VERSION = 0
BPF_PATCHLEVEL = 0
BPF_EXTRAVERSION = 1

MAKEFLAGS += --no-print-directory

ifeq ($(srctree),)
srctree := $(patsubst %/,%,$(dir $(CURDIR)))
srctree := $(patsubst %/,%,$(dir $(srctree)))
srctree := $(patsubst %/,%,$(dir $(srctree)))
#$(info Determined 'srctree' to be $(srctree))
endif

# Makefiles suck: This macro sets a default value of $(2) for the
# variable named by $(1), unless the variable has been set by
# environment or command line. This is necessary for CC and AR
# because make sets default values, so the simpler ?= approach
# won't work as expected.
define allow-override
  $(if $(or $(findstring environment,$(origin $(1))),\
            $(findstring command line,$(origin $(1)))),,\
    $(eval $(1) = $(2)))
endef

# Allow setting CC and AR, or setting CROSS_COMPILE as a prefix.
$(call allow-override,CC,$(CROSS_COMPILE)gcc)
$(call allow-override,AR,$(CROSS_COMPILE)ar)

INSTALL = install

# Use DESTDIR for installing into a different root directory.
# This is useful for building a package. The program will be
# installed in this directory as if it was the root directory.
# Then the build tool can move it later.
DESTDIR ?=
DESTDIR_SQ = '$(subst ','\'',$(DESTDIR))'

include $(srctree)/tools/scripts/Makefile.arch

ifeq ($(LP64), 1)
  libdir_relative = lib64
else
  libdir_relative = lib
endif

prefix ?= /usr/local
libdir = $(prefix)/$(libdir_relative)
man_dir = $(prefix)/share/man
man_dir_SQ = '$(subst ','\'',$(man_dir))'

export man_dir man_dir_SQ INSTALL
export DESTDIR DESTDIR_SQ

include ../../scripts/Makefile.include

# copy a bit from Linux kbuild

ifeq ("$(origin V)", "command line")
  VERBOSE = $(V)
endif
ifndef VERBOSE
  VERBOSE = 0
endif

INCLUDES = -I. -I$(srctree)/tools/include -I$(srctree)/tools/arch/$(ARCH)/include/uapi -I$(srctree)/tools/include/uapi -I$(srctree)/tools/perf

export prefix libdir src obj

# Shell quotes
libdir_SQ = $(subst ','\'',$(libdir))
libdir_relative_SQ = $(subst ','\'',$(libdir_relative))

LIB_FILE = libbpf.a libbpf.so

VERSION		= $(BPF_VERSION)
PATCHLEVEL	= $(BPF_PATCHLEVEL)
EXTRAVERSION	= $(BPF_EXTRAVERSION)

OBJ		= $@
N		=

LIBBPF_VERSION = $(BPF_VERSION).$(BPF_PATCHLEVEL).$(BPF_EXTRAVERSION)

# Set compile option CFLAGS
ifdef EXTRA_CFLAGS
  CFLAGS := $(EXTRA_CFLAGS)
else
  CFLAGS := -g -Wall
endif

ifeq ($(feature-libelf-mmap), 1)
  override CFLAGS += -DHAVE_LIBELF_MMAP_SUPPORT
endif

ifeq ($(feature-reallocarray), 0)
  override CFLAGS += -DCOMPAT_NEED_REALLOCARRAY
endif

# Append required CFLAGS
override CFLAGS += $(EXTRA_WARNINGS)
override CFLAGS += -Werror -Wall
override CFLAGS += -fPIC
override CFLAGS += $(INCLUDES)

ifeq ($(VERBOSE),1)
  Q =
else
  Q = @
endif

# Disable command line variables (CFLAGS) override from top
# level Makefile (perf), otherwise build Makefile will get
# the same command line setup.
MAKEOVERRIDES=

all:

export srctree OUTPUT CC LD CFLAGS V
include $(srctree)/tools/build/Makefile.include

BPF_IN    := $(OUTPUT)libbpf-in.o
LIB_FILE := $(addprefix $(OUTPUT),$(LIB_FILE))

CMD_TARGETS = $(LIB_FILE)

TARGETS = $(CMD_TARGETS)

all: fixdep
	$(Q)$(MAKE) all_cmd

all_cmd: $(CMD_TARGETS)

$(BPF_IN): force
	@(test -f ../../include/uapi/linux/bpf.h -a -f ../../../include/uapi/linux/bpf.h && ( \
	(diff -B ../../include/uapi/linux/bpf.h ../../../include/uapi/linux/bpf.h >/dev/null) || \
	echo "Warning: Kernel ABI header at 'tools/include/uapi/linux/bpf.h' differs from latest version at 'include/uapi/linux/bpf.h'" >&2 )) || true
	@(test -f ../../include/uapi/linux/bpf_common.h -a -f ../../../include/uapi/linux/bpf_common.h && ( \
	(diff -B ../../include/uapi/linux/bpf_common.h ../../../include/uapi/linux/bpf_common.h >/dev/null) || \
	echo "Warning: Kernel ABI header at 'tools/include/uapi/linux/bpf_common.h' differs from latest version at 'include/uapi/linux/bpf_common.h'" >&2 )) || true
	@(test -f ../../include/uapi/linux/netlink.h -a -f ../../../include/uapi/linux/netlink.h && ( \
	(diff -B ../../include/uapi/linux/netlink.h ../../../include/uapi/linux/netlink.h >/dev/null) || \
	echo "Warning: Kernel ABI header at 'tools/include/uapi/linux/netlink.h' differs from latest version at 'include/uapi/linux/netlink.h'" >&2 )) || true
	@(test -f ../../include/uapi/linux/if_link.h -a -f ../../../include/uapi/linux/if_link.h && ( \
	(diff -B ../../include/uapi/linux/if_link.h ../../../include/uapi/linux/if_link.h >/dev/null) || \
	echo "Warning: Kernel ABI header at 'tools/include/uapi/linux/if_link.h' differs from latest version at 'include/uapi/linux/if_link.h'" >&2 )) || true
	$(Q)$(MAKE) $(build)=libbpf

$(OUTPUT)libbpf.so: $(BPF_IN)
	$(QUIET_LINK)$(CC) --shared $^ -o $@

$(OUTPUT)libbpf.a: $(BPF_IN)
	$(QUIET_LINK)$(RM) $@; $(AR) rcs $@ $^

define do_install
	if [ ! -d '$(DESTDIR_SQ)$2' ]; then		\
		$(INSTALL) -d -m 755 '$(DESTDIR_SQ)$2';	\
	fi;						\
	$(INSTALL) $(if $3,-m $3,) $1 '$(DESTDIR_SQ)$2'
endef

install_lib: all_cmd
	$(call QUIET_INSTALL, $(LIB_FILE)) \
		$(call do_install,$(LIB_FILE),$(libdir_SQ))

install_headers:
	$(call QUIET_INSTALL, headers) \
		$(call do_install,bpf.h,$(prefix)/include/bpf,644); \
		$(call do_install,libbpf.h,$(prefix)/include/bpf,644);
		$(call do_install,btf.h,$(prefix)/include/bpf,644);

install: install_lib

clean:
	$(call QUIET_CLEAN, libbpf) $(RM) *.o *~ $(TARGETS) *.a *.so .*.d .*.cmd \
		$(RM) LIBBPF-CFLAGS

PHONY += force
force:

# Declare the contents of the .PHONY variable as phony.  We keep that
# information in a variable so we can use it in if_changed and friends.
.PHONY: $(PHONY)
