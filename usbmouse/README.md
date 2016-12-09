# *usbmouse* prototyping work
This directory contains some of the intial prototyping work completed to help us figure out how to build and work with the usbmouse kernel module

**NOTE: usbmouse.c from linux kernel version v4.8 ([Link][1])**
[1]: https://github.com/torvalds/linux/blob/v4.8/drivers/hid/usbhid/usbmouse.c

# Changes:
Line 36: 
Changed #include "../hid-ids.h" to #include "hid-ids.h"

Added hid-ids.h from linux source tree

run `make` to build usbmouse.c
run `make clean` to clean this directory
