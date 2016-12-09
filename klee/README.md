#Klee Setup
This directory sets up the klee container used to run the klee environment

To create environment, run:
~~~~
make build
~~~~

To test enviroment, run:
~~~~
make test1.results test2.results
~~~~

To clean directory, run:
~~~~
make clean
~~~~
NOTE: This does not remove the created *.results* files

To run experimental results in docker container, run:
~~~~
make usbmouse.results
~~~~
