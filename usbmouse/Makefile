# Choose to use clang as your compiler
HOSTCC		:= clang
CC		:= clang
CPP		:= $(CC) -E

LINUXFLAGS	= CPP="$(CPP)" CC="$(CC)" HOSTCC="$(HOSTCC)"

obj-m += usbmouse.o
all:
	$(MAKE) -C /lib/modules/$(shell uname -r)/build M=$(PWD) $(LINUXFLAGS) modules
clean:
	$(MAKE) -C /lib/modules/$(shell uname -r)/build M=$(PWD) $(LINUXFLAGS) clean
