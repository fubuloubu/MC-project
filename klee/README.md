#Klee Experiments
This directory contains the rules for running Klee experiments
*NOTE*: Tested using Ubuntu 16.04.1 LTS

# Instructions
Ensure you have docker installed:
~~~~
sudo apt install docker
~~~~

First create docker container:
~~~~
make build
~~~~

To obtain experimental results in docker container, run:
~~~~
make usbmouse.results
~~~~
*NOTE*: This will fail

To clean directory, run:
~~~~
make clean
~~~~

To test enviroment, run:
~~~~
make test1.results test2.results
~~~~
