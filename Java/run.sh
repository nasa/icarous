#!/bin/bash

MODE=active
EXEC=icarous.jar
JSSCLIB=lib/jssc-2.8.0.jar
DAIDALUS=lib/ICAROUS.jar
SITL_HOST=localhost
SITL_INPUT_PORT=14551
COM_HOST=localhost
COM_INPUT_PORT=14552
COM_OUTPUT_PORT=14553
PX4_PORT=/dev/tty01
#GS_MASTER=127.0.0.1:$COM_OUTPUT_PORT
GS_MASTER=/dev/ttyUSB0
GPIO_PORT=23
RADIO_SERIAL_PORT=/dev/ttyUSB1
RADIO_SOCKET_IN=$COM_OUTPUT_PORT
RADIO_SOCKET_OUT=$COM_INPUT_PORT

if [ "$1" == 'SITL' ];then
   echo "Launching ICAROUS with SITL"
   $PRE java -cp $EXEC:$JSSCLIB:$DAIDALUS launch \
	-v \
	--sitl $SITL_HOST $SITL_INPUT_PORT \
	--com $COM_HOST $COM_INPUT_PORT $COM_OUTPUT_PORT \
	--mode $MODE ${POST}
   
elif [ "$1" == 'PX4' ];then
    echo "Launching ICAROUS with Pixhawk"
    
    if [ "$2" == "nohup" ];then
	nohup java -cp $EXEC BB_SAFEGUARD $GPIO_PORT $COM_INPUT_PORT &
	nohup java -cp $EXEC:$JSSCLIB:$DAIDALUS launch \
	 -v \
	 --px4 $PX4_PORT \
	 --com $COM_HOST $COM_INPUT_PORT $COM_OUTPUT_PORT \
	 --mode $MODE > pxout.txt &
    else
	java -cp $EXEC:$JSSCLIB:$DAIDALUS launch \
	 -v \
	 --px4 $PX4_PORT \
	 --com $COM_HOST $COM_INPUT_PORT $COM_OUTPUT_PORT \
	 --mode $MODE
    fi
    
elif [ "$1" == 'GS' ];then
    echo "Launching Ground station test"
    mavproxy.py --master=$GS_MASTER --map --console --load-module geofence,traffic

elif [ "$1" == 'RADIO' ];then
    echo "Launching radio module"
    java -cp $EXEC:$JSSCLIB radio $RADIO_SOCKET_IN $RADIO_SOCKET_OUT $RADIO_SERIAL_PORT
    
elif [ "$1" == 'SAFE' ];then
    echo "Launching SAFEGUARD listener"
    java -cp $EXEC BB_SAFEGUARD $GPIO_PORT $COM_INPUT_PORT
    
else
    echo "run.sh [ SITL | PX4 | GS | RADIO | SAFE ]"    
fi

