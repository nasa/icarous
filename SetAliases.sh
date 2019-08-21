#!/bin/bash

# Aliases and functions to quickly build/run icarous

ICROOT=$(pwd)
ICBUILD=$ICROOT/build
ICEXE=$ICROOT/cFS/bin/cpu1
ICSIM=$ICROOT/sim

alias icroot="cd $ICROOT"
alias icbuild="mkdir -p $ICBUILD; cd $ICBUILD"
alias icexe="cd $ICEXE"
alias icsim="mkdir -p $ICSIM; cd $ICSIM"

alias icstart="icexe && ./core-cpu1 -C 1 -I 0"
alias icgs="icsim && $ICROOT/Scripts/runGS.sh"
alias icmake="icbuild && make cpu1-install -j8"
alias icup="ps -A | grep core-cpu1"

# Run Icarous with a given startup script and CPU id
# use: icrun [script name] [CPU id]
icrun(){
  icexe
  # Set startup script
  if [ $# -ge 1 ]
  then
    icset $1
  fi
  # Set CPU id and spacecraft id
  c=1
  if [ $# -ge 2 ]
  then
    c=$2
  fi
  let i=$2-1
  # Start Icarous
  echo "./core-cpu1 -C $c -I $i"
  ./core-cpu1 -C $c -I $i
}

# Set Icarous startup script
# use: icset [script name]
icset(){
  home=$PWD
  icexe
  rm cf/cfe_es_startup.scr
  ln -s $ICROOT/cFS/apps/Icarous_defs/cpu1_cfe_es_startup_$1.scr cf/cfe_es_startup.scr
  cd $home
  echo "Icarous start script set to $1"
}
