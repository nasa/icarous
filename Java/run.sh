#!/bin/bash

if [ "$1" == 'SITL' ];then
   echo "Launching ICAROUS for SITL"
   java -cp bin:lib/jssc-2.8.0.jar:lib/FormalATMj/FormalATM.jar launch -v --sitl 14551 --com localhost 14552 14553
elif [ "$1" == 'RSITL' ];then
    echo "Launching ICAROUS for SITL radio"
    java -cp bin:lib/jssc-2.8.0.jar:lib/FormalATMj/FormalATM.jar launch -v --sitl 14551 --radio /dev/ttyUSB0
elif [ "$1" == 'PX4' ];then
    echo "Launching ICAROUS for Pixhawk"
    java -cp bin:lib/jssc-2.8.0.jar:lib/FormalATMj/FormalATM.jar launch -v --px4 /dev/ttyO1 --com localhost 14552 14553 --bc 230.1.1.1 5555
elif [ "$1" == 'GS' ];then
    echo "Launching Ground station test"
    mavproxy.py --master=127.0.0.1:14553 --map --load-module geofence,traffic
elif [ "$1" == 'GPSdebug' ];then
   echo "GPS debugger"
   java -cp bin:lib/jssc-2.8.0.jar:lib/FormalATMj/FormalATM.jar DebugGPS --px4 /dev/ttyO1
elif [ "$1" == 'VOLDEMORT' ];then
   echo "Testing message receipt from VOLDEMORT"
   java -cp bin:lib/jssc-2.8.0.jar:lib/FormalATMj/FormalATM.jar Voldemort_test -v --sitl 14551 --com 14552 --bc 230.1.1.1 5555
else
    echo "run.sh [ SITL | COMBOX | PX4 | GPSdebug | VOLDEMORT ]"
fi

