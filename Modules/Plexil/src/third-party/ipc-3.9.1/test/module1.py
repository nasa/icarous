###########################################################################
# PROJECT: IPC (Interprocess Communication) Package
#
# (c) Copyright 2011 Reid Simmons.  All rights reserved.
#
# FILE: module1.py
#
# ABSTRACT: Test program for Python version of IPC.
#           Publishes: MSG1, QUERY1
#           Subscribes to: MSG2
#           Behavior: Sends MSG1 whenever an "m" is typed at the terminal;
#                     Sends a QUERY1 whenever an "r" is typed;
#                     Quits the program when a 'q' is typed.
#                     Should be run in conjunction with module2.
#
# $Revision: 2.2 $
# $Date: 2011/08/16 21:33:44 $
# $Author: reids $
#
# Copyright (c) 2008, Carnegie Mellon University
#     This software is distributed under the terms of the 
#     Simplified BSD License (see ipc/LICENSE.TXT)
#
# REVISION HISTORY
#
# $Log: module1.py,v $
# Revision 2.2  2011/08/16 21:33:44  reids
# Took out extraneous semi-colons
#
# Revision 2.1  2011/08/16 16:00:09  reids
# Adding Python test programs
#
###########################################################################

import sys
import IPC
from module import *

def msg2Handler (msgRef, callData, clientData) :
  print "msg2Handler: Receiving %s (%s) [%s] " % \
	(IPC.IPC_msgInstanceName(msgRef), callData, clientData)

done = False

def stdinHnd (fd, clientData) :
  global done
  input = sys.stdin.readline()

  if (input[0] == 'q' or input[0] == 'Q') :
    IPC.IPC_disconnect()
    done = True
  elif (input[0] == 'm' or input[0] == 'M') :
    i1 = 42
    print "\n  IPC_publishData(%s, %d)" % (MSG1, i1)
    IPC.IPC_publishData(MSG1, i1)
  elif (input[0] == 'r' or input[0] == 'R') :
    t1 = T1()
    t1.i1 = 666
    t1.status = SendVal
    t1.matrix = ((0.0, 1.0, 2.0), (1.0, 2.0, 3.0))
    t1.d1 = 3.14159
    print "\n  IPC_queryResponseData(%s, %s, IPC_WAIT_FOREVER, %s)" % \
          (QUERY1, t1, T1.__name__)
    (r1, ret) = IPC.IPC_queryResponseData(QUERY1, t1, IPC.IPC_WAIT_FOREVER, T1)
    print "\n  Received response"
    IPC.IPC_printData(IPC.IPC_msgFormatter(RESPONSE1), sys.stdout, r1)
  else :
    print "stdinHnd [%s]: Received %s" % (clientData, input),

def handlerChangeHnd (msgName, num, clientData) :
  print "HANDLER CHANGE: %s: %d" % (msgName, num)

def handlerChangeHnd2 (msgName, num, clientData) :
  print "HANDLER CHANGE2: %s: %d" % (msgName, num)

def connect1Hnd (moduleName, clientData) :
  print "CONNECT1: Connection from %s" % moduleName
  print "          Confirming connection (%d)" % \
        IPC.IPC_isModuleConnected(moduleName)

def connect2Hnd (moduleName, clientData) :
  print "CONNECT2: Connection from %s" % moduleName
  print "          Number of handlers: %d" % IPC.IPC_numHandlers(MSG1)

first = True

def disconnect1Hnd (moduleName, clientData) :
  global first
  print "DISCONNECT:", moduleName
  if (first) : IPC.IPC_unsubscribeConnect(connect1Hnd)
  else : IPC.IPC_unsubscribeConnect(connect2Hnd)
  if (first) : IPC.IPC_unsubscribeHandlerChange(MSG1, handlerChangeHnd2)
  else : IPC.IPC_unsubscribeHandlerChange(MSG1, handlerChangeHnd)
  first = False

def main () :
  global done, first
  done = False; first = True

  # Connect to the central server
  print "\nIPC.IPC_connect(%s)" % MODULE1_NAME
  print IPC.IPC_connect, sys.stdin, sys.stdin.fileno()
  IPC.IPC_connect(MODULE1_NAME)
  print "HERE1"

  IPC.IPC_subscribeConnect(connect1Hnd, None)
  IPC.IPC_subscribeConnect(connect2Hnd, None)
  IPC.IPC_subscribeDisconnect(disconnect1Hnd, None)

  # Define the named formats that the modules need
  print "\nIPC.IPC_defineFormat(%s, %s)" % (T1_NAME, T1_FORMAT)
  IPC.IPC_defineFormat(T1_NAME, T1_FORMAT)
  print "\nIPC.IPC_defineFormat(%s, %s)" % (T2_NAME, T2_FORMAT)
  IPC.IPC_defineFormat(T2_NAME, T2_FORMAT)
  
  # Define the messages that this module publishes
  print "\nIPC.IPC_defineMsg(%s, IPC_VARIABLE_LENGTH, %s)" %(MSG1, MSG1_FORMAT)
  IPC.IPC_defineMsg(MSG1, IPC.IPC_VARIABLE_LENGTH, MSG1_FORMAT)

  IPC.IPC_subscribeHandlerChange(MSG1, handlerChangeHnd, None)
  IPC.IPC_subscribeHandlerChange(MSG1, handlerChangeHnd2, None)

  print "\nIPC.IPC_defineMsg(%s, IPC_VARIABLE_LENGTH, %s)" % \
        (QUERY1, QUERY1_FORMAT)
  IPC.IPC_defineMsg(QUERY1, IPC.IPC_VARIABLE_LENGTH, QUERY1_FORMAT)
  IPC.IPC_subscribeHandlerChange(QUERY1, handlerChangeHnd, None)

  # Subscribe to the messages that this module listens to.
  # NOTE: No need to subscribe to the RESPONSE1 message, since it is a
  #       response to a query, not a regular subscription!
  print "\nIPC.IPC_subscribeData(%s, msg2Handler, %s)" % (MSG2, MODULE1_NAME)
  IPC.IPC_subscribe(MSG2, msg2Handler, MODULE1_NAME)

  # Subscribe a handler for tty input.
  #  Typing "q" will quit the program; Typing "m" will send MSG1;
  #  Typing "r" will send QUERY1 ("r" for response)
  print "\nIPC.IPC_subscribeFD(%d, stdinHnd, %s)" % \
        (sys.stdin.fileno(), MODULE1_NAME)
  IPC.IPC_subscribeFD(sys.stdin.fileno(), stdinHnd, MODULE1_NAME)

  print "\nType 'm' to send %s; Type 'r' to send %s; Type 'q' to quit" % \
        (MSG1, QUERY1)

  while (not done) : IPC.IPC_listen(250)

  IPC.IPC_disconnect()
