#!/bin/bash

GITHOME=$(pwd)

export JAVA_HOME="/usr/lib/jvm/java-8-openjdk-amd64"
export PLEXIL_HOME="$GITHOME/Modules/Plexil"
export OSAL_HOME="$GITHOME/cFS/cFE/osal"
export DYLD_LIBRARY_PATH=$DYLD_LIBRARY_PATH:"$GITHOME/Modules/lib"
export DYLD_LIBRARY_PATH=$DYLD_LIBRARY_PATH:"$GITHOME/Modules/Plexil/lib"
#export	OSPL_HOME="$GITHOME/icarous/Modules/SBD/opensplice/HDE/x86.linux"

# Check that paths exist
#[ ! -d $JAVA_HOME ] && echo "JAVA_HOME ($JAVA_HOME) doesn't exist!"
#[ ! -d $PLEXIL_HOME ] && echo "PLEXIL_HOME ($PLEXIL_HOME) doesn't exist!"
#[ ! -d $OSAL_HOME ] && echo "OSAL_HOME ($OSAL_HOME) doesn't exist!"
