###########################################################################
# PROJECT: IPC (Interprocess Communication) Package
#
# (c) Copyright 2011 Reid Simmons.  All rights reserved.
#
# FILE: module3.py
#
# ABSTRACT: Test program for Python version of IPC.
#           Subscribes to: MSG1, MSG2
#           Behavior: Prints out the message data received.
#                     Exits when 'q' is typed at terminal
#                     Should be run in conjunction with module1 and module2
#
# $Revision: 2.1 $
# $Date: 2011/08/16 16:00:10 $
# $Author: reids $
#
# Copyright (c) 2008, Carnegie Mellon University
#     This software is distributed under the terms of the 
#     Simplified BSD License (see ipc/LICENSE.TXT)
#
# REVISION HISTORY
#
# $Log: module3.py,v $
# Revision 2.1  2011/08/16 16:00:10  reids
# Adding Python test programs
#
################################################################/

import sys
import IPC
from module import *

def msg1Handler_3 (msgRef, callData, clientData) :
  (i1, ret) = IPC.IPC_unmarshallData(IPC.IPC_msgInstanceFormatter(msgRef),
                                     callData)
  print "msg1Handler: Receiving %s (%d) [%s]" % \
        (IPC.IPC_msgInstanceName(msgRef), i1, clientData)
  IPC.IPC_freeByteArray(callData)

def msg2Handler_3 (msgRef, callData, clientData) :
  (str1, ret) = IPC.IPC_unmarshallData(IPC.IPC_msgInstanceFormatter(msgRef),
                                       callData)
  print "msg2Handler: Receiving %s (%s) [%s]" % \
        (IPC.IPC_msgInstanceName(msgRef), str1, clientData)
  IPC.IPC_freeByteArray(callData)

done = False

def stdinHnd_3 (fd, clientData) :
  global done
  input = sys.stdin.readline()

  if (input[0] == 'q' or input[0] == 'Q') :
    IPC.IPC_disconnect()
    done = True
  else :
    print "stdinHnd [%s]: Received %s" % (clientData, input)

def main () :
  global done
  done = False
  # Connect to the central server
  print "\nIPC.IPC_connect(%s)" % MODULE3_NAME
  IPC.IPC_connect(MODULE3_NAME)

  # Subscribe to the messages that this module listens to.
  print "\nIPC_subscribe(%s, %s, %s)" % \
        (MSG1, msg1Handler_3.__name__, MODULE3_NAME)
  IPC.IPC_subscribe(MSG1, msg1Handler_3, MODULE3_NAME)

  # Subscribe to the messages that this module listens to.
  print "\nIPC_subscribe(%s, %s, %s)" % \
        (MSG2, msg2Handler_3.__name__, MODULE3_NAME)
  IPC.IPC_subscribe(MSG2, msg2Handler_3, MODULE3_NAME)

  # Subscribe a handler for tty input. Typing "q" will quit the program.
  print "\nIPC_subscribeFD(%d, stdinHnd_3, %s)" % \
        (sys.stdin.fileno(), MODULE3_NAME)
  IPC.IPC_subscribeFD(sys.stdin.fileno(), stdinHnd_3, MODULE3_NAME)

  print "\nType 'q' to quit"
  while (not done) : IPC.IPC_listen(250)

  IPC.IPC_disconnect()
