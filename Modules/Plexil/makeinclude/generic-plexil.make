# -*- Mode: Makefile -*-
# File: $SVNROOT/makeinclude/generic-plexil.make
# Note: Generic makefile for plexilisp and standard plexil.  Compiles
#       and validates *.pl{i,e} and lib/*.pl{i,e}.

# For use in running agents from a makefile which includes this one
RUN_AGENTS = $(PLEXIL_HOME)/scripts/run-agents

# Find the .pli and .ple plans in ./ and lib/
PLANS = $(wildcard *.pli *.ple lib/*.pli lib/*.ple)

# Find all of the .plx targets implied by PLANS
TARGETS = $(filter %.plx, $(PLANS:%.ple=%.plx) $(PLANS:%.pli=%.plx))

# The default target for this file (leave all for others)
plx: $(TARGETS)

# Plexilisp rule
%.plx: %.pli
	plexilisp $<
	@xmllint --noout --schema $(PLEXIL_HOME)/schema/core-plexil.xsd $*.plx
	@$(RM) $*.epx*

# Standard Plexil rule
%.plx: %.ple
	plexilc $<
	@xmllint --noout --schema $(PLEXIL_HOME)/schema/core-plexil.xsd $*.plx
	@$(RM) $*.epx*

# Default clean targets.  Can be redefined in makefile which includes this file.
# This idiom allows makefiles which include this one to add more rules to clean, etc.
DIRS = . lib test

_dust:
	for dir in $(DIRS) ; do $(RM) $${dir}/*.{epx,last} ; done
dust: _dust

_clean: dust
	for dir in $(DIRS) ; do $(RM) $${dir}/*.plx ; $(RM) core.* ; done
clean: _clean

_cleaner: clean
	for dir in $(DIRS) ; do $(RM) $${dir}/*~ ; done
cleaner: _cleaner

plexil-targets:
	@echo "plans:   " $(PLANS)
	@echo "targets: " $(TARGETS)

.PHONY: _dust dust _clean clean _cleaner cleaner plexil-targets

# EOF
