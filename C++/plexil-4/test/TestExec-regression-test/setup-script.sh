#!/bin/sh

# This is hacky, and no longer needed, since the debugging file can now
# be passed to the executive.

touch Debug.cfg
cp -f Debug.cfg .SavedDebug.cfg
cp -f .TestDebug.cfg Debug.cfg
rm -f RegressionResults
touch RegressionResults
#rm tempRegressionResults
touch tempRegressionResults
if [ ! -d output ]
then
 rm -f output
 mkdir output
fi
rm -f output/RUN_*.out
