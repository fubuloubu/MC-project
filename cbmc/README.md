# CBMC Experiments
This directory contains the rules for running CBMC experiments
*NOTE*: Tested using Ubuntu 16.04.1 LTS

# Instructions
Ensure you have CBMC installed:
~~~~
sudo apt install cbmc
~~~~

To obtain results for all 5 function calls in usbmouse, run:
~~~~
make all
~~~~

*NOTE*: usb\_mouse\_probe() will fail.
To view failures, run:
~~~~
grep FAILED usb_mouse_probe.results
~~~~

To clean up results, run:
~~~~
make clean
~~~~
