#!/bin/bash

MODE=active
EXEC=lib/icarous.jar
JSSCLIB=lib/jssc-2.8.0.jar
FORMALATM=lib/FormalATM.jar
SITL_HOST=localhost
SITL_INPUT_PORT=14551
COM_HOST=localhost
COM_INPUT_PORT=14552
COM_OUTPUT_PORT=14553
PX4_PORT=/dev/ttyO1
PX4_BAUD=57600
#GS_MASTER=192.42.142.83:$COM_OUTPUT_PORT
GS_MASTER=127.0.0.1:$COM_OUTPUT_PORT
#GS_MASTER=/dev/ttyUSB0
GPIO_PORT=23
RADIO_SERIAL_PORT=/dev/ttyUSB0
RADIO_SOCKET_IN=$COM_OUTPUT_PORT
RADIO_SOCKET_OUT=$COM_INPUT_PORT
RADIO_BAUD=57600

if [ "$1" == 'SITL' ];then
   echo "Launching ICAROUS with SITL"
   $PRE java -cp $EXEC:$JSSCLIB:$FORMALATM launch \
	-v \
	--sitl $SITL_HOST $SITL_INPUT_PORT \
	--com $COM_HOST $COM_INPUT_PORT $COM_OUTPUT_PORT \
	--mode $MODE ${POST}
   
elif [ "$1" == 'PX4' ];then
    echo "Launching ICAROUS with Pixhawk"
    
    if [ "$2" == "nohup" ];then
	nohup java -cp $EXEC BB_SAFEGUARD $GPIO_PORT $COM_INPUT_PORT &
	nohup java -cp $EXEC:$JSSCLIB:$FORMALATM launch \
	 -v \
	 --px4 $PX4_PORT $PX4_BAUD\
	 --com $COM_HOST $COM_INPUT_PORT $COM_OUTPUT_PORT \
	 --mode $MODE > pxout.txt &
    else
	java -cp $EXEC:$JSSCLIB:$FORMALATM launch \
	 -v \
	 --px4 $PX4_PORT $PX4_BAUD\
	 --com $COM_HOST $COM_INPUT_PORT $COM_OUTPUT_PORT \
	 --mode $MODE
    fi
elif [ "$1" == 'SITLR' ];then
    echo "Launching ICAROUS with SITL and radio"
    java -cp $EXEC:$JSSCLIB:$FORMALATM launch \
	 -v \
	 --sitl $SITL_HOST $SITL_INPUT_PORT \
	 --radio $RADIO_SERIAL_PORT $RADIO_BAUD \
	 --mode $MODE
    
elif [ "$1" == 'PX4R' ];then
    echo "Launching ICAROUS with Pixhawk"
    java -cp $EXEC:$JSSCLIB:$FORMALATM launch \
	 -v \
	 --px4 $PX4_PORT $PX4_BAUD\
	 --radio $RADIO_SERIAL_PORT $RADIO_BAUD \
	 --mode $MODE
    
elif [ "$1" == 'GS' ];then
    echo "Launching Ground station test"
    mavproxy.py --master=$GS_MASTER --map --console --load-module geofence,traffic --mav10 --dialect=icarous

elif [ "$1" == 'RADIO' ];then
    echo "Launching radio module"
    java -cp $EXEC:$JSSCLIB radio $RADIO_SOCKET_IN $RADIO_SOCKET_OUT $RADIO_SERIAL_PORT $RADIO_BAUD 
    
elif [ "$1" == 'SAFE' ];then
    echo "Launching SAFEGUARD listener"
    java -cp $EXEC BB_SAFEGUARD $GPIO_PORT $COM_INPUT_PORT
elif [ "$1" == 'DaidalusPolyExample' ];then
    java -cp $EXEC DaidalusPolyExample params/DaidalusQuadConfig.txt
else
    echo "run.sh [ SITL | PX4 | GS | RADIO | SAFE | DaidalusPolyExample ]"    
fi
