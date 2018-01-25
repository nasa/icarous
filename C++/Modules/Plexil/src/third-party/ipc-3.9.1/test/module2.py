###########################################################################
# PROJECT: IPC (Interprocess Communication) Package
#
# (c) Copyright 2011 Reid Simmons.  All rights reserved.
#
# FILE: module2.py
#
# ABSTRACT: Test program for Python version of IPC.
#           Publishes: MSG2
#           Subscribes to: MSG1, QUERY1
#           Responds with: RESPONSE1
#           Behavior: Listens for MSG1 and prints out message data.
#                     When QUERY1 is received, publishes MSG1 and
#                     responds to the query with RESPONSE1.
#                     Exits when 'q' is typed at terminal.
#                     Should be run in conjunction with module1
#
# $Revision: 2.1 $
# $Date: 2011/08/16 16:00:09 $
# $Author: reids $
#
# Copyright (c) 2008, Carnegie Mellon University
#     This software is distributed under the terms of the 
#     Simplified BSD License (see ipc/LICENSE.TXT)
#
# REVISION HISTORY
#
# $Log: module2.py,v $
# Revision 2.1  2011/08/16 16:00:09  reids
# Adding Python test programs
#
################################################################/

import sys
import IPC
from module import *

def msg1Handler (msgRef, callData, clientData) :
  print "msg1Handler: Receiving %s (%d) [%s] " % \
	(IPC.IPC_msgInstanceName(msgRef), callData, clientData)

def queryHandler (msgRef, t1, clientData) :
  print "queryHandler: Receiving %s [%s]",  \
	(IPC.IPC_msgInstanceName(msgRef), clientData)
  IPC.IPC_printData(IPC.IPC_msgInstanceFormatter(msgRef), sys.stdout, t1)

  # Publish this message -- all subscribers get it
  str1 = "Hello, world"
  print '\n  IPC.IPC_publishData(%s, "%s")' % (MSG2, str1)
  IPC.IPC_publishData(MSG2, str1)

  t2 =  T2()
  t2.str1 = str1
  # Variable length array of one element 
  t2.t1 = [T1()]
  t2.t1[0] = t1
  t2.count = 1
  t2.status = ReceiveVal

  # Respond with this message -- only the query handler gets it 
  print "\n  IPC.IPC_respondData(%s, %s, %s)" % (msgRef, RESPONSE1, t2)
  IPC.IPC_respondData(msgRef, RESPONSE1, t2)

done = False

def stdinHnd (fd, clientData) :
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
  print "\nIPC.IPC_connect(%s)" % MODULE2_NAME
  IPC.IPC_connect(MODULE2_NAME)

  # Define the messages that this module publishes
  print "\nIPC.IPC_defineMsg(%s, IPC_VARIABLE_LENGTH, %s)" % \
        (MSG2, MSG2_FORMAT)
  IPC.IPC_defineMsg(MSG2, IPC.IPC_VARIABLE_LENGTH, MSG2_FORMAT)

  print "\nIPC.IPC_defineMsg(%s, IPC_VARIABLE_LENGTH, %s)" % \
        (RESPONSE1, RESPONSE1_FORMAT)
  IPC.IPC_defineMsg(RESPONSE1, IPC.IPC_VARIABLE_LENGTH, RESPONSE1_FORMAT)

  # Subscribe to the messages that this module listens to
  print "\nIPC.IPC_subscribeData(%s,%s, %s)" % \
        (MSG1, msg1Handler.__name__, MODULE2_NAME)
  IPC.IPC_subscribeData(MSG1, msg1Handler, MODULE2_NAME)

  print "\nIPC.IPC_subscribeData(%s, %s, %s, %s)" % \
        (QUERY1 , queryHandler.__name__, MODULE2_NAME, T1.__name__)
  IPC.IPC_subscribeData(QUERY1, queryHandler, MODULE2_NAME, T1)

  # Subscribe a handler for tty input. Typing "q" will quit the program.
  print "\nIPC_subscribeFD(%d, stdinHnd, %s)" % \
        (sys.stdin.fileno(), MODULE2_NAME)
  IPC.IPC_subscribeFD(sys.stdin.fileno(), stdinHnd, MODULE2_NAME)

  print "\nType 'q' to quit"
  while (not done) : IPC.IPC_listen(250)

  IPC.IPC_disconnect()
