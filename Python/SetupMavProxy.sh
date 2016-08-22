#!/bin/bash
MAVPROXY_PATH="$1"
echo "Path to mavproxy modules: $MAVPROXY_PATH/MAVProxy/MAVProxy/modules/"
echo "Creating symbolic links"
for files in *.py;do
    ln -s "$PWD/$files" "$MAVPROXY_PATH/MAVProxy/MAVProxy/modules/$files"
done;

echo "Installing MAVProxy"
ICAROUS_dir=$PWD
cd $MAVPROXY_PATH/MAVProxy/
python setup.py build install --user
cd $ICAROUS_dir


