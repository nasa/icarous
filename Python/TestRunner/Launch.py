#!/bin/python

import subprocess
import os
import os.path
import sys
import time
import signal

TestIndex = sys.argv[1]

# Open output file
f1 = open('output.txt','w')

# Launch Icarous
ic = subprocess.Popen(["./core-cpu1"],stdout=f1)

time.sleep(300)

ic.kill()

print "Finished running playback"

command = 'grep "Plexil status:" output.txt > PlexilOutput.log'
subprocess.call(command,shell=True)
val = subprocess.call(["diff","-Z","PlexilOutput.log","../ram/IClog/TestCase"+TestIndex+"/PlexilOutput.log"])

assert (val==0), "Plexil output check failed"

