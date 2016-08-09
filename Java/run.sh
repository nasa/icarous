#!/bin/bash

if [ "$1" == 'SITL' ];then
   echo "Launching ICAROUS for SITL"
   java -cp bin:lib/jssc-2.8.0.jar:lib/FormalATMj/FormalATM.jar SITL_test -v --sitl 14551 --com 14552 --bc 230.1.1.1 5555
elif [ "$1" == 'COMBOX' ];then
   echo "Launching COMBOX"
    if [ -z "$2" -o ! -f "$2" ]; then
	mission="./params/ComInput.txt"
    else
	mission=$2
    fi
    echo "Mission: $mission"
    java -cp bin:lib/jssc-2.8.0.jar:lib/FormalATMj/FormalATM.jar ComBox $mission
elif [ "$1" == 'PX4' ];then
    echo "Launching ICAROUS for Pixhawk"
    java -cp bin:lib/jssc-2.8.0.jar:lib/FormalATMj/FormalATM.jar PX4Test -v --px4 /dev/ttyO1 --com 14552 --bc 230.1.1.1 5555
elif [ "$1" == 'GS' ];then
    echo "Launching Ground station test"
    java -cp bin:lib/jssc-2.8.0.jar:lib/FormalATMj/FormalATM.jar GStest -v --px4 /dev/ttyO1 --com 14552 --bc 230.1.1.1 5555
elif [ "$1" == 'GPSdebug' ];then
   echo "GPS debugger"
   java -cp bin:lib/jssc-2.8.0.jar:lib/FormalATMj/FormalATM.jar DebugGPS --px4 /dev/ttyO1
elif [ "$1" == 'VOLDEMORT' ];then
   echo "Testing message receipt from VOLDEMORT"
   java -cp bin:lib/jssc-2.8.0.jar:lib/FormalATMj/FormalATM.jar Voldemort_test -v --sitl 14551 --com 14552 --bc 230.1.1.1 5555
else
    echo "run.sh [ SITL | COMBOX | PX4 | GPSdebug | VOLDEMORT ]"
fi

