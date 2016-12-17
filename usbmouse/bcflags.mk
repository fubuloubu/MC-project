# Start with fresh variable
BCFLAGS=

# These are all the flags that linux make constructed, but are now unused
#BCFLAGS+=-Wp,-MD,$(STARTDIR)/.usbmouse.o.d

# These flags are unsupported by clang 3.8
#BCFLAGS+=-falign-jumps=1
#BCFLAGS+=-falign-loops=1
#BCFLAGS+=-fno-delete-null-pointer-checks

# These flags are unsupported by clang 3.4
#BCFLAGS+=-fstack-protector-strong

# These are all the flags that linux make constructed, but seem required

# Do not search the standard system directories 
# or compiler builtin directories for include files
BCFLAGS+=-nostdinc

# Include clang headers as system headers
# NOTE: Bash env used so it executes in running enviroment
CLANG_VER=$$(clang --version | grep version | grep -o "[0-9].[0-9].[0-9]")
LLVM_VER =$$(clang --version | grep version | grep -o "[0-9].[0-9].[0-9]" | grep -o "^[0-9].[0-9]")
BCFLAGS+=-isystem /usr/lib/llvm-$(LLVM_VER)/lib/clang/$(CLANG_VER)/include

# Building in this directory
# NOTE: Bash env used so it executes in running enviroment
LINUXDIR=/usr/src/linux-headers-$$(uname -r)

# Include directories for compiling linux
INCLUDE_FLAGS =-I$(LINUXDIR)/include
INCLUDE_FLAGS+=-I$(LINUXDIR)/include/uapi
INCLUDE_FLAGS+=-I$(LINUXDIR)/include/generated/uapi
INCLUDE_FLAGS+=-I$(LINUXDIR)/arch/x86/include
INCLUDE_FLAGS+=-I$(LINUXDIR)/arch/x86/include/generated
INCLUDE_FLAGS+=-I$(LINUXDIR)/arch/x86/include/uapi
INCLUDE_FLAGS+=-I$(LINUXDIR)/arch/x86/include/generated/uapi
INCLUDE_FLAGS+=-I$(LINUXDIR)/ubuntu/include
BCFLAGS+=$(INCLUDE_FLAGS)

# Linux Pre-compiled Header NOTE: Must be after other includes
BCFLAGS+=-include $(LINUXDIR)/include/linux/kconfig.h

# Macro flags
MACRO_FLAGS =-D__KERNEL__
MACRO_FLAGS+=-DCONFIG_X86_X32_ABI
MACRO_FLAGS+=-DCONFIG_AS_CFI=1
MACRO_FLAGS+=-DCONFIG_AS_CFI_SIGNAL_FRAME=1
MACRO_FLAGS+=-DCONFIG_AS_CFI_SECTIONS=1
MACRO_FLAGS+=-DCONFIG_AS_FXSAVEQ=1
MACRO_FLAGS+=-DCONFIG_AS_SSSE3=1
MACRO_FLAGS+=-DCONFIG_AS_CRC32=1
MACRO_FLAGS+=-DCONFIG_AS_AVX=1
MACRO_FLAGS+=-DCONFIG_AS_AVX2=1
MACRO_FLAGS+=-DCONFIG_AS_SHA1_NI=1
MACRO_FLAGS+=-DCONFIG_AS_SHA256_NI=1
MACRO_FLAGS+=-DMODULE
MACRO_FLAGS+=-D"KBUILD_STR(s)=\#s"
MACRO_FLAGS+=-D"KBUILD_BASENAME=KBUILD_STR(usbmouse)"
MACRO_FLAGS+=-D"KBUILD_MODNAME=KBUILD_STR(usbmouse)"
BCFLAGS+=$(MACRO_FLAGS)

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
