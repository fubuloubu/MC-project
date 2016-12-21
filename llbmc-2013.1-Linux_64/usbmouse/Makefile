obj-m := usbmouse.o

# Added so that preprocessor and assembly objects
# get saved in current directory
EXTRA_CFLAGS += -save-temps=obj
# Undefine this macro so the "asm goto..."
# optimizations are removed, since clang can't parse them
EXTRA_CFLAGS += -UCC_HAVE_ASM_GOTO

BUILDIR=/lib/modules/$(shell uname -r)/build
PWD=$(shell pwd)

all:
	$(MAKE) -C $(BUILDIR) M=$(PWD) modules
	# Some files went under weird names...
	mv .tmp_usbmouse.i usbmouse.i
	mv .tmp_usbmouse.s usbmouse.s
clean:
	$(MAKE) -C $(BUILDIR) M=$(PWD) clean
	rm -f *.i
