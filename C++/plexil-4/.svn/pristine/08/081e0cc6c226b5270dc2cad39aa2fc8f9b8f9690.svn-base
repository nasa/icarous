# Copyright (c) 2006-2012, Universities Space Research Association (USRA).
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#     * Redistributions of source code must retain the above copyright
#       notice, this list of conditions and the following disclaimer.
#     * Redistributions in binary form must reproduce the above copyright
#       notice, this list of conditions and the following disclaimer in the
#       documentation and/or other materials provided with the distribution.
#     * Neither the name of the Universities Space Research Association nor the
#       names of its contributors may be used to endorse or promote products
#       derived from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY USRA ``AS IS'' AND ANY EXPRESS OR IMPLIED
# WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
# MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL USRA BE LIABLE FOR ANY DIRECT, INDIRECT,
# INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
# BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
# OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
# ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
# TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
# USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

DEBUG_CFG	:= .TestDebug.cfg

EXEC_PROG	:= $(PLEXIL_HOME)/src/apps/TestExec/TestExec -d $(DEBUG_CFG)
REGRESSION_PL	:= regression.pl

# Parser error detection tests
PARSER_ERROR_TESTS	:= $(wildcard plans/parser-tests/*.plx)

# Tests which run to success and require no script
EMPTY_SCRIPT		:= scripts/empty.psx

EMPTY_SCRIPT_TESTS	:= AncestorReferenceTest array2 array5 array6 array9 assign-failure-with-conflict \
 concat1 contention1 contention3 \
 double-invariant-assignment \
 empty1 empty2 empty3 empty4 empty-string1 \
 failure-type1 failure-type2 failure-type3 failure-type4 \
 grandparent-access-plan \
 interface1 iteration-ended1 invariant1 isKnown1 \
 maxTest minTest modulo1 \
 repeat1 repeat3 repeat4 \
 simple-assignment skip1 skip2 \
 test-abs-sqrt-plan test-node-name-scope-hack \
 test-node-name-scope test-repeat-condition timepoint-variable-construction-order \
 variables1 var-priority-with-exit var-priority-with-skip \
 whitespace1 \
 uninitialized-assignment \
 array-in-loop

EMPTY_SCRIPT_VALID_TESTS := assign-to-parent-invariant assign-to-parent-exit non-local-exit

LIBRARY_TESTS	:=  Increment-test Increment-test2 library-call-with-array

# Tests which run to success and require a script of the same name
SAME_NAME_SCRIPT_TESTS	:= array1 array3 array4 array8 atomic-assignment \
 boolean1 \
 change-lookup-test command1 command2 command3 command4 command5 concat2 conjuncts conjuncts1 \
 lookup1 lookup2 lookup3 \
 repeat2 repeat5 repeat7 repeat8 \
 site-survey \
 test-end-condition test-timepoint \
 update-lookup-test update-test

# Resource arbitration tests, which require comparing the output with a known standard
RESOURCE_ARBITRATION_TESTS	:= resource1 resource1-repeatCond \
 resource2-equalPriority \
 resource3-ackRel resource3-denyHP resource3-deny2HP \
 resource4-hvm resource4-hvm-repeatCond \
 resource-renewable1 \
 NonUnaryResources

# Simple-drive tests
SIMPLE_DRIVE_PLAN	:= simple-drive
SIMPLE_DRIVE_SCRIPTS	:= single-drive double-drive

all: exec-regression-tests

exec-regression-tests: run-regression-suite

setup: 
	@-$(RM) RegressionResults tempRegressionResults
	@-if [ ! -d output ] ; then $(RM) output ; mkdir output ; fi
	@-$(RM) output/*.out
	@date > RegressionResults
	@echo
	@echo --------------------- SUMMARY OF FAILED TESTS ---------------------
	@echo

end-report: run-all-tests
	@echo
	@echo ------------------- END SUMMARY OF FAILED TESTS ------------------- 
	@echo -- for complete test results see file: RegressionResults 
	@echo

run-tests-with-setup: setup run-all-tests

run-all-tests: parser-tests
run-all-tests: empty-script-tests empty-script-valid-tests same-name-script-tests
run-all-tests: simple-drive-tests command-handles-tests resource-arbitration-tests
run-all-tests: library-call-with-array

run-regression-suite: run-tests-with-setup end-report

# These are tests of parser error detection.

parser-tests: setup
	@. $(PLEXIL_HOME)/scripts/plexil-setup.sh && \
	for tst in $(PARSER_ERROR_TESTS) ; do \
		tstname=`basename $${tst%.plx}`; \
		echo $${tstname} >> tempRegressionResults ; \
		if $(EXEC_PROG) -p $${tst} -s $(EMPTY_SCRIPT) > output/$${tstname}.out 2>&1 ; then \
			echo $${tstname}: Failed to detect expected parser error ; \
		fi ; \
	done

# NOTE: Some of the following sets are self-checking tests:
# success of the root node implies success.  No 'valid'
# comparison file is checked.

empty-script-tests: setup
	@. $(PLEXIL_HOME)/scripts/plexil-setup.sh && \
	for tst in $(EMPTY_SCRIPT_TESTS) ; do \
		echo $${tst} >> tempRegressionResults ; \
		$(EXEC_PROG) -p plans/$${tst}.plx -s $(EMPTY_SCRIPT) > output/$${tst}.out 2>> tempRegressionResults ; \
		perl $(REGRESSION_PL) output/$${tst}.out ; \
	done

empty-script-valid-tests: setup
	@. $(PLEXIL_HOME)/scripts/plexil-setup.sh && \
	for tst in $(EMPTY_SCRIPT_VALID_TESTS) ; do \
		echo $${tst} >> tempRegressionResults ; \
		$(EXEC_PROG) -p plans/$${tst}.plx -s $(EMPTY_SCRIPT) > output/$${tst}.out ; \
		perl $(REGRESSION_PL) output/$${tst}.out valid/RUN_$${tst}_empty-script.valid ; \
	done

same-name-script-tests: setup
	@. $(PLEXIL_HOME)/scripts/plexil-setup.sh && \
	for tst in $(SAME_NAME_SCRIPT_TESTS) ; do \
		echo $${tst} >> tempRegressionResults ; \
		$(EXEC_PROG) -p plans/$${tst}.plx -s scripts/$${tst}.psx > output/$${tst}.out 2>> tempRegressionResults ; \
		perl $(REGRESSION_PL) output/$${tst}.out ; \
	done

simple-drive-tests: setup
	@. $(PLEXIL_HOME)/scripts/plexil-setup.sh && \
	for scrpt in $(SIMPLE_DRIVE_SCRIPTS) ; do \
		echo $${scrpt} >> tempRegressionResults ; \
		$(EXEC_PROG) -p plans/$(SIMPLE_DRIVE_PLAN).plx -s scripts/$${scrpt}.psx > output/$${scrpt}.out 2>> tempRegressionResults ; \
		perl $(REGRESSION_PL) output/$${scrpt}.out ; \
	done

command-handles-tests: setup
	@. $(PLEXIL_HOME)/scripts/plexil-setup.sh && \
	for openclosed in open closed ; do \
		for plan in a1 a3 ; do \
			planname=simple-$${openclosed}loop-command-$${plan} ; \
			for script in a1 a3 ; do \
				scriptname=simple-$${openclosed}loop-command-$${script} ; \
				logname=$${planname}_$${script}.out ; \
				echo $${planname}_$${scriptname} >> tempRegressionResults ; \
				$(EXEC_PROG) -p plans/$${planname}.plx -s scripts/$${scriptname}.psx > output/$${logname} 2>> tempRegressionResults ; \
				perl $(REGRESSION_PL) output/$${logname} valid/RUN_$${planname}_$${scriptname}-script.valid ; \
			done ; \
		done ; \
	done
	@. $(PLEXIL_HOME)/scripts/plexil-setup.sh && \
	planname=simple-openloop-command-nopost ; \
	scriptname=simple-openloop-command-a3 ; \
	echo $${planname} >> tempRegressionResults ; \
	$(EXEC_PROG) -p plans/$${planname}.plx -s scripts/$${scriptname}.psx > output/$${planname}.out 2>> tempRegressionResults ; \
	perl $(REGRESSION_PL) output/$${planname}.out valid/RUN_$${planname}_$${scriptname}-script.valid
	@. $(PLEXIL_HOME)/scripts/plexil-setup.sh && \
	planname=simple-closedloop-command-multipleAck ; \
	scriptname=$${planname} ; \
	echo $${planname} >> tempRegressionResults ; \
	$(EXEC_PROG) -p plans/$${planname}.plx -s scripts/$${scriptname}.psx > output/$${planname}.out 2>> tempRegressionResults ; \
	perl $(REGRESSION_PL) output/$${planname}.out valid/RUN_$${planname}_$${scriptname}-script.valid

resource-arbitration-tests: setup
	@. $(PLEXIL_HOME)/scripts/plexil-setup.sh && \
	for tst in $(RESOURCE_ARBITRATION_TESTS) ; do \
		echo $${tst} >> tempRegressionResults ; \
		$(EXEC_PROG) -p plans/$${tst}.plx -s scripts/$${tst}.psx > output/$${tst}.out 2>> tempRegressionResults ; \
		perl $(REGRESSION_PL) output/$${tst}.out valid/RUN_$${tst}_$${tst}-script.valid ; \
	done

library-call-with-array: setup
	@. $(PLEXIL_HOME)/scripts/plexil-setup.sh && \
	echo $@ >> tempRegressionResults ; \
	$(EXEC_PROG) -p plans/$@.plx -L plans -s $(EMPTY_SCRIPT) > output/$@.out 2>> tempRegressionResults ; \
	perl $(REGRESSION_PL) output/$@.out

clean:
	-$(RM) RegressionResults tempRegressionResults
	-$(RM) -r output

# Targets that should be made every time they are invoked
.PHONY: setup header summary

.PHONY: empty-script-tests empty-script-valid-tests same-name-script-tests

.PHONY: simple-drive-tests command-handles-tests resource-arbitration-tests
