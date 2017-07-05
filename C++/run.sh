#!/bin/bash

MODE=active
SITL_HOST=127.0.0.1
SITL_INPUT_PORT=14551
COM_HOST=127.0.0.1
COM_INPUT_PORT=14552
COM_OUTPUT_PORT=14553
PX4_PORT=/dev/ttyO1
PX4_BAUD=57600
#GS_MASTER=192.42.142.110:$COM_OUTPUT_PORT
GS_MASTER=127.0.0.1:$COM_OUTPUT_PORT
#GS_MASTER=/dev/ttyUSB0
GPIO_PORT=23
RADIO_SERIAL_PORT=/dev/ttyUSB0
RADIO_SOCKET_IN=$COM_OUTPUT_PORT
RADIO_SOCKET_OUT=$COM_INPUT_PORT
RADIO_BAUD=57600

if [ "$1" == 'SITL' ];then
   echo "Launching ICAROUS with SITL"
   $PRE ./icarous --verbose \
	--sitlhost $SITL_HOST --sitlin $SITL_INPUT_PORT \
	--gshost $COM_HOST --gsin $COM_INPUT_PORT --gsout $COM_OUTPUT_PORT \
	--mode $MODE --config $2 $3 ${POST}
   
elif [ "$1" == 'PX4' ];then
    echo "Launching ICAROUS with Pixhawk"
       ./icarous --verbose \
	--px4 $PX4_PORT --px4baud $PX4_BAUD \
	--gshost $COM_HOST --gsin $COM_INPUT_PORT --gsout $COM_OUTPUT_PORT \
	--mode --config $2 $3 $MODE
        
elif [ "$1" == 'PX4R' ];then
    echo "Launching ICAROUS with Pixhawk and Radio"
    ./icarous --verbose \
	--px4 $PX4_PORT --px4baud $PX4_BAUD \
	--radio $RADIO_SERIAL_PORT --radiobaud $RADIO_BAUD \
	--mode $MODE --config $2 $3
    
elif [ "$1" == 'GS' ];then
    echo "Launching Ground station test"
    mavproxy.py --master=$GS_MASTER --map --console --load-module geofence,traffic,icparams --mav10 --dialect=icarous

else
    echo "run.sh [ SITL | PX4 | GS | RADIO ]"    
fi

