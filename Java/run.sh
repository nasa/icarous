#!/bin/bash

if [ "$1" == 'SITL' ];then
   echo "Launching ICAROUS for SITL"
   java -cp bin:lib/jssc-2.8.0.jar:lib/FormalATM.jar SITL_test -v --sitl 14551 --com 14552 --bc 230.1.1.1 5555
fi

if [ "$1" == 'COMBOX' ];then
   echo "Launching COMBOX"
    if [ -z "$2" -o ! -f "$2" ]; then
	mission="./params/ComInput.txt"
    else
	mission=$2
    fi
    java -cp bin:lib/jssc-2.8.0.jar:lib/FormalATM.jar ComBox $mission
fi

if [ "$1" == 'PX4' ];then
    echo "Launching ICAROUS for Pixhawk"
    java -cp bin:lib/jssc-2.8.0.jar:lib/FormalATM.jar PX4Test -v --px4 /dev/ttyO1 --com 14552 --bc 230.1.1.1 5555
fi

if [ "$1" == 'TestVoldemort' ];then
   echo "Testing message receipt from VOLDEMORT"
   java -cp bin:lib/jssc-2.8.0.jar:lib/FormalATM.jar Voldemort_test -v --sitl 14551 --com 14552 --bc 230.1.1.1 5555
fi

