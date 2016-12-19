# *usbmouse* prototyping work
This directory contains some of the intial prototyping work completed to help us figure out how to build and work with the usbmouse kernel module

*NOTE*: usbmouse.c from linux kernel version v4.8 ([Link][1])
[1]: https://github.com/torvalds/linux/blob/v4.8/drivers/hid/usbhid/usbmouse.c

# Changes to original usbmouse.c:
*Line 36*: 
Changed #include "../hid-ids.h" to #include "hid-ids.h".
Copied hid-ids.h from linux source tree ([Link][2]).
[2]: https://github.com/torvalds/linux/blob/v4.8/drivers/hid/hid-ids.h

*Lines 62, 101, 112, 119, 216*:
Removed static keyword for all usb\_mouse\_\* functions.

run `make all` to generate *usbmouse.i*, the pre-processed source
run `make clean` to clean this directory
