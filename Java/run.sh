#!/bin/bash

if [ "$1" == 'SITL' ];then
   echo "Launching ICAROUS for SITL"
   java -cp bin:lib/jssc-2.8.0.jar:lib/FormalATM.jar SITL_test 14551 14552
fi;

if [ "$1" == 'COMBOX' ];then
   echo "Launching COMBOX"
   java -cp bin:lib/jssc-2.8.0.jar:lib/FormalATM.jar ComBox ./params/ComInput.txt
fi;

if [ "$1" == 'PX4' ];then
   echo "Launcing ICAROUS for Pixhawk"
   java -cp bin:lib/jssc-2.8.0.jar:lib/FormalATM.jar PX4Test /dev/ttyO1 14552
fi;

if [ "$1" == 'TestVoldemort' ];then
   echo "Testing message receipt from VOLDEMORT"
   java -cp bin:lib/jssc-2.8.0.jar:lib/FormalATM.jar Voldemort_test 14551 14552
fi;

