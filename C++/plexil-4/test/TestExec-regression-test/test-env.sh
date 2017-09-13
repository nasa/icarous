#! /usr/bin/env bash
# Environment setup for TestExec regression tests

# Copyright (c) 2006-2015, Universities Space Research Association (USRA).
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

# Defining library path redundantly for both Mac and Linux

# Linux
LD_LIBRARY_PATH=${PLEXIL_HOME}/lib

# Mac
DYLD_LIBRARY_PATH=$LD_LIBRARY_PATH
export LD_LIBRARY_PATH DYLD_LIBRARY_PATH

TEST_DIR=${PLEXIL_HOME}/test/TestExec-regression-test

EMPTY_SCRIPT=scripts/empty.psx
REGRESSION_PL=regression.pl
TEST_DEBUG_CFG=.TestDebug.cfg

if [ -z "$EXEC_PROG" ]
then
    EXEC_PROG=${PLEXIL_HOME}/src/apps/TestExec/TestExec
fi

export EMPTY_SCRIPT EXEC_PROG REGRESSION_PL TEST_DEBUG_CFG TEST_DIR
