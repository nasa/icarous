#! /usr/bin/env bash
# How to cross-compile Plexil with buildroot

# Copyright (c) 2006-2015, Universities Space Research Association (USRA).
#  All rights reserved.
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

# Substitute the appropriate triplet.
# ARM on newer buildroot versions
TARGET=arm-unknown-linux-uclibcgnueabi
# ARM on older buildroot versions
#TARGET=arm-linux-uclibcgnueabi

# Substitute the appropriate paths.
PLEXIL_HOME=/home/JRandomUser/src/plexil-4
BUILDROOT_HOME=/home/JRandomUser/src/buildroot/buildroot-2012.08
# Newer buildroot versions
TOOLCHAIN_ROOT=${BUILDROOT_HOME}/output/host
# Older buildroot versions
#TOOLCHAIN_ROOT=${BUILDROOT_HOME}/output/staging

# Uncomment this to build module tests and TestExec regression tests.
#BUILD_TESTS=1

# These should not need to be changed.
TARGET_GCC=${TOOLCHAIN_ROOT}/usr/bin/${TARGET}-gcc
TARGET_GXX=${TOOLCHAIN_ROOT}/usr/bin/${TARGET}-g++
TARGET_NM=${TOOLCHAIN_ROOT}/usr/bin/${TARGET}-nm
BUILD_ROOT=${TOOLCHAIN_ROOT}/output/build/plexil-4.0
TARGET_ROOT=${BUILDROOT_HOME}/output/target
TARGET_INCLUDES=${TARGET_ROOT}/usr/include
TARGET_SHARE=${TARGET_ROOT}/usr/share

TEST_CONFIGURE_OPTS=
if (-n "$BUILD_TESTS")
then
    TEST_CONFIGURE_OPTS='-enable-module-tests -enable-test-exec'
fi

export PATH=${PLEXIL_HOME}/src:${TOOLCHAIN_ROOT}/usr/bin:${TOOLCHAIN_ROOT}/usr/sbin:${PATH}

# Exit on error
set -e

# Build PLEXIL for target
mkdir -p $BUILD_ROOT
cd $BUILD_ROOT
${PLEXIL_HOME}/src/configure --target=$TARGET --host=$TARGET \
 CC=$TARGET_GCC \
 CXX=$TARGET_GXX \
 NM=$TARGET_NM \
 --prefix=${TARGET_ROOT}/usr \
 $TEST_CONFIGURE_OPTS
make

# Copy files to target filesystem
make install
mkdir -p ${TARGET_SHARE}/plexil
cd ${TARGET_SHARE}/plexil
svn export --force https://plexil.svn.sourceforge.net/svnroot/plexil/branches/plexil-4/scripts
if ( -n "$BUILD_TESTS" )
then
    svn export --force https://plexil.svn.sourceforge.net/svnroot/plexil/branches/plexil-4/examples
    svn export --force https://plexil.svn.sourceforge.net/svnroot/plexil/branches/plexil-4/test
    # The utils and exec module tests require certain files in the working directory
    mkdir -p test/utils-module-tests test/exec-module-tests
    cp ${PLEXIL_HOME}/src/utils/test/debug*.cfg test/utils-module-tests/
    cp ${PLEXIL_HOME}/src/exec/test/Debug.cfg test/exec-module-tests/
fi
