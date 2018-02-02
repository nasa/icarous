File: $SVNROOT/examples/multi-exec/agents/test/t3-readme.txt

In t3-cmd.pli (using t1-sim.pli), using Janson's "workaround" to get
values to update in the Plan Viewer, I find that the string value
"UNKNOWN" doesn't work in at least some cases.  In particular, neither

  (end-condition (not (= (stringvar "return") "UNKNOWN")))

nor

  (end-condition (not (= (stringvar "return") (stringval "UNKNOWN"))))

work as expected.  On the other hand, "Unknown", and every other
string I've tried work fine.

% cd plexil/examples/multi-exec/agents/tests
% make
% xterm ipc &
% xterm -v -p t1-sim.plx -c sim.xml
% plexilexec -v -b -p t3-cmd.plx -c cmd.xml

or

% cd plexil/examples/multi-exec/agents/tests
% run-agents [-v] t1-sim t3-cmd

t3-cmd.plx never completes due to the above end-condition (line 25).
