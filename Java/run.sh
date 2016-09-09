#!/bin/bash

EXEC=icarous.jar
JSSCLIB=lib/jssc-2.8.0.jar
DAIDALUS=lib/FormalATMj/FormalATM.jar
SITL_HOST=localhost
SITL_INPUT_PORT=14551
COM_HOST=localhost
COM_INPUT_PORT=14552
COM_OUTPUT_PORT=14553
MODE=active
PX4_PORT=/dev/tty01
GS_MASTER=127.0.0.1:$COM_OUTPUT_PORT
#GS_MASTER=/dev/ttyUSB0
GPIO_PORT=23

if [ "$1" == 'SITL' ];then
   echo "Launching ICAROUS with SITL"
   java -cp $EXEC:$JSSCLIB:$DAIDALUS launch \
	-v \
	--sitl $SITL_HOST $SITL_INPUT_PORT \
	--com $COM_HOST $COM_INPUT_PORT $COM_OUTPUT_PORT \
	--mode $MODE
   
elif [ "$1" == 'PX4' ];then
    echo "Launching ICAROUS with Pixhawk"
    java -cp $EXEC:$JSSCLIB:$DAIDALUS launch \
	 -v \
	 --px4 $PX4_PORT \
	 --com $COM_HOST $COM_INPUT_PORT $COM_OUTPUT_PORT \
	 --mode $MODE
    
elif [ "$1" == 'GS' ];then
    echo "Launching Ground station test"
    mavproxy.py --master=$GS_MASTER --map --console --load-module geofence,traffic
    
elif [ "$1" == 'SAFE' ];then
    echo "Launching SAFEGUARD listener"
    java -cp $EXEC BB_SAFEGUARD $GPIO_PORT $COM_INPUT_PORT
    
else
    echo "run.sh [ SITL | PX4 | GS | SAFE ]"    
fi

