File: $SVNROOT/examples/multi-exec/agents/test/t1-readme.txt

Plexil Viewer doesn't update the "return" variable in "MasterPlan"
node in t1-cmd.pli.

% cd plexil/examples/multi-exec/agents/tests
% make
% xterm ipc &
% xterm -v -p t1-sim.plx -c sim.xml
% plexilexec -v -b -p t1-cmd.plx -c cmd.xml

or

% cd plexil/examples/multi-exec/agents/tests
% run-agents -v t1-sim t1-cmd

In t1-cmd.plx, the following variable doesn't update:

  MasterPlan: return

