# -*- Mode: Makefile -*-
# File: $SVNROOT/examples/multi-exec/agents/test/makefile
# Note: makefile agents/test

OPTS=

include $(PLEXIL_HOME)/makeinclude/generic-plexil.make

t1:
	$(RUN_AGENTS) $(OPTS) t1-sim t1-cmd

# intermittently errors out
t2:
	$(RUN_AGENTS) $(OPTS) t1-sim t2-cmd

# t3 hangs (but should not)
t3:
	$(RUN_AGENTS) $(OPTS) t1-sim t3-cmd

# t4 get a run-time error and hangs (but should not)
t4:
	$(RUN_AGENTS) $(OPTS) t4-sim t1-cmd

.PHONY: t1 t2 t3 t4

# EOF
