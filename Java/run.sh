#!/bin/bash

if [ "$1" == 'SITL' ];then
   echo "Launching SITL"
   java -cp bin:lib/jssc-2.8.0.jar:lib/FormalATM.jar SITL_test 14551 14552
fi;

if [ "$1" == 'COMBOX' ];then
   echo "Launching COMBOX"
   java -cp bin:lib/jssc-2.8.0.jar:lib/FormalATM.jar ComBox ./bin/ComInput.txt
fi;

