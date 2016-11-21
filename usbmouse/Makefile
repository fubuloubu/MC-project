# Start with fresh variable
BCFLAGS=

# These are all the flags that linux make constructed, but are now unused
#BCFLAGS+=-Wp,-MD,$(STARTDIR)/.usbmouse.o.d

# These flags are unsupported by clang 3.8
#BCFLAGS+=-falign-jumps=1
#BCFLAGS+=-falign-loops=1
#BCFLAGS+=-fno-delete-null-pointer-checks

# These are all the flags that linux make constructed, but seem required

# Do not search the standard system directories 
# or compiler builtin directories for include files
BCFLAGS+=-nostdinc

# Include clang headers as system headers
BCFLAGS+=-isystem /usr/lib/llvm-3.8/lib/clang/3.8.0/include

# Building in this directory
LINUXDIR=/usr/src/linux-headers-4.4.0-47-generic

# Include directories for compiling linux
BCFLAGS+=-I$(LINUXDIR)/arch/x86/include
BCFLAGS+=-I$(LINUXDIR)/arch/x86/include/generated/uapi
BCFLAGS+=-I$(LINUXDIR)/arch/x86/include/generated
BCFLAGS+=-I$(LINUXDIR)/arch/x86/include/uapi
BCFLAGS+=-I$(LINUXDIR)/arch/x86/include/generated/uapi
BCFLAGS+=-I$(LINUXDIR)/include
BCFLAGS+=-I$(LINUXDIR)/include/uapi
BCFLAGS+=-I$(LINUXDIR)/include/generated/uapi
BCFLAGS+=-I$(LINUXDIR)/ubuntu/include

# Linux Pre-compiled Header NOTE: Must be after other includes
BCFLAGS+=-include $(LINUXDIR)/include/linux/kconfig.h

# Macro flags
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

# Machine Dependant options
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

# Clang configuration flags
BCFLAGS+=-fno-pie
BCFLAGS+=-fno-strict-aliasing
BCFLAGS+=-fno-common
BCFLAGS+=-fno-pie
BCFLAGS+=-funit-at-a-time
BCFLAGS+=-fno-asynchronous-unwind-tables
BCFLAGS+=-fstack-protector-strong
BCFLAGS+=-fno-omit-frame-pointer
BCFLAGS+=-fno-optimize-sibling-calls
BCFLAGS+=-fno-strict-overflow

BCFLAGS+=-Qunused-arguments
BCFLAGS+=-std=gnu89
BCFLAGS+=-pipe
BCFLAGS+=--param=allow-store-data-races=0
BCFLAGS+=-pg

# Optimization level
BCFLAGS+=-O2

# Warnings
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

# Added to suppress unused function warnings
BCFLAGS+=-Wno-unused-function

# Compiler stage (originally -c) TODO: Try -S
BCFLAGS+=-c

# Generate Bytecode (and store in file -o)
BCFLAGS+=-emit-llvm

%.bc: %.c
	@echo "  CLANG $<"
	@clang $(BCFLAGS) -o $@ $^

.PHONY: clean
clean:
	@echo "  CLEAN clang"
	@rm -f usbmouse.bc 
