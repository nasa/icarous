nohup ./run.sh $1 > px4-out.txt &
sleep 5
nohup ./run.sh COMBOX > cb-out.txt &
