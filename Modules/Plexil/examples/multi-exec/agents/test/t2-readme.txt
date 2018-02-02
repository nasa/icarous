File: $SVNROOT/examples/multi-exec/agents/test/t2-readme.txt

In t2-cmd.pli (using t1-sim.pli), Plexil Viewer doesn't update
"return" variable in "MasterPlan" node for the first assignment (line
19), then updates the second assignment (line 22) with the value
"one" rather than the "two" passed in the assignement.

% cd plexil/examples/multi-exec/agents/tests
% make
% xterm ipc &
% xterm -v -p t1-sim.plx -c sim.xml
% plexilexec -v -b -p t2-cmd.plx -c cmd.xml

or

% cd plexil/examples/multi-exec/agents/tests
% run-agents [-v] t1-sim t2-cmd
