# Starting from Current Directory
STARTDIR=$(shell pwd)

# Building in this directory
BUILDDIR=/usr/src/linux-headers-4.4.0-47-generic

# Starter
BCFLAGS=

# These are all the flags that linux make constructed, but are now unused
#BCFLAGS+=-Wp,-MD,$(STARTDIR)/.usbmouse.o.d

# These are all the flags that linux make constructed, but seem required
BCFLAGS+=-isystem /usr/lib/llvm-3.8/bin/../lib/clang/3.8.0/include
BCFLAGS+=-I./arch/x86/include
BCFLAGS+=-Iarch/x86/include/generated/uapi
BCFLAGS+=-Iarch/x86/include/generated
BCFLAGS+=-Iinclude
BCFLAGS+=-I./arch/x86/include/uapi
BCFLAGS+=-Iarch/x86/include/generated/uapi
BCFLAGS+=-I./include/uapi
BCFLAGS+=-Iinclude/generated/uapi
BCFLAGS+=-Iubuntu/include
BCFLAGS+=-include ./include/linux/kconfig.h

BCFLAGS+=-mno-sse
BCFLAGS+=-mno-mmx
BCFLAGS+=-mno-sse2
BCFLAGS+=-mno-3dnow
BCFLAGS+=-mno-avx
BCFLAGS+=-m64
BCFLAGS+=-mtune=generic
BCFLAGS+=-mno-red-zone
BCFLAGS+=-mcmodel=kernel
BCFLAGS+=-mno-global-merge

BCFLAGS+=-fno-pie
BCFLAGS+=-fno-strict-aliasing
BCFLAGS+=-fno-common
BCFLAGS+=-fno-pie
BCFLAGS+=-falign-jumps=1
BCFLAGS+=-falign-loops=1
BCFLAGS+=-funit-at-a-time
BCFLAGS+=-fno-asynchronous-unwind-tables
BCFLAGS+=-fno-delete-null-pointer-checks
BCFLAGS+=-fstack-protector-strong
BCFLAGS+=-fno-omit-frame-pointer
BCFLAGS+=-fno-optimize-sibling-calls
BCFLAGS+=-fno-strict-overflow

BCFLAGS+=-Qunused-arguments
BCFLAGS+=-nostdinc
BCFLAGS+=-std=gnu89
BCFLAGS+=-pipe
BCFLAGS+=--param=allow-store-data-races=0
BCFLAGS+=-pg
BCFLAGS+=-O2

BCFLAGS+=-Wno-unknown-warning-option
BCFLAGS+=-Wall
BCFLAGS+=-Wundef
BCFLAGS+=-Wstrict-prototypes
BCFLAGS+=-Wno-trigraphs
BCFLAGS+=-Werror-implicit-function-declaration
BCFLAGS+=-Wno-format-security
BCFLAGS+=-Wno-sign-compare
BCFLAGS+=-Wno-maybe-uninitialized
BCFLAGS+=-Wframe-larger-than=1024
BCFLAGS+=-Wno-unused-variable
BCFLAGS+=-Wno-format-invalid-specifier
BCFLAGS+=-Wno-gnu
BCFLAGS+=-Wno-tautological-compare
BCFLAGS+=-Wdeclaration-after-statement
BCFLAGS+=-Wno-pointer-sign
BCFLAGS+=-Werror=implicit-int
BCFLAGS+=-Werror=strict-prototypes
BCFLAGS+=-Werror=date-time
BCFLAGS+=-Wno-initializer-overrides
BCFLAGS+=-Wno-unused-value
BCFLAGS+=-Wno-format
BCFLAGS+=-Wno-unknown-warning-option
BCFLAGS+=-Wno-sign-compare
BCFLAGS+=-Wno-format-zero-length
BCFLAGS+=-Wno-uninitialized

BCFLAGS+=-D__KERNEL__
BCFLAGS+=-DCONFIG_X86_X32_ABI
BCFLAGS+=-DCONFIG_AS_CFI=1
BCFLAGS+=-DCONFIG_AS_CFI_SIGNAL_FRAME=1
BCFLAGS+=-DCONFIG_AS_CFI_SECTIONS=1
BCFLAGS+=-DCONFIG_AS_FXSAVEQ=1
BCFLAGS+=-DCONFIG_AS_SSSE3=1
BCFLAGS+=-DCONFIG_AS_CRC32=1
BCFLAGS+=-DCONFIG_AS_AVX=1
BCFLAGS+=-DCONFIG_AS_AVX2=1
BCFLAGS+=-DCONFIG_AS_SHA1_NI=1
BCFLAGS+=-DCONFIG_AS_SHA256_NI=1
BCFLAGS+=-DMODULE
BCFLAGS+=-D"KBUILD_STR(s)=\#s"
BCFLAGS+=-D"KBUILD_BASENAME=KBUILD_STR(usbmouse)"
BCFLAGS+=-D"KBUILD_MODNAME=KBUILD_STR(usbmouse)"

BCFLAGS+=-c

usbmouse.bc:
	cd $(BUILDDIR) && clang $(BCFLAGS) -emit-llvm -o $(STARTDIR)/usbmouse.bc $(STARTDIR)/usbmouse.c

.PHONY: clean
clean:
	rm -f usbmouse.bc 
