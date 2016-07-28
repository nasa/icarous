ps -ax | fgrep PX4
ps -ax | fgrep STIL
ps -ax | fgrep ComBox
ps -ax | fgrep sim_vehicle
ps -ax | fgrep mavproxy.py
ps -ax | fgrep FormalATM
echo "..."
pkill -f FormalATM
