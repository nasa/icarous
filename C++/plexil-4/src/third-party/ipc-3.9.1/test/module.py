##############################################################################
# PROJECT: IPC (Interprocess Communication) Package
#
# (c) Copyright 2011 Reid Simmons.  All rights reserved.
#
# FILE: module.py
#
# ABSTRACT: Common defs for module1, module2 and module3 test programs
#
# $Revision: 2.1 $
# $Date: 2011/08/16 16:00:07 $
# $Author: reids $
#
# Copyright (c) 2008, Carnegie Mellon University
#     This software is distributed under the terms of the 
#     Simplified BSD License (see ipc/LICENSE.TXT)
#
# REVISION HISTORY
#
# $Log: module.py,v $
# Revision 2.1  2011/08/16 16:00:07  reids
# Adding Python test programs
#
##############################################################################

import IPC

WaitVal    = 0
SendVal    = 1
ReceiveVal = 2
ListenVal  = 3

class T1(IPC.IPCdata) :
  _fields = ('i1', 'status', 'matrix', 'd1')

class T2(IPC.IPCdata) :
  _fields = ('str1', 'count', ('t1', T1), 'status')

T1_NAME  = "T1"
# First form of "enum". 3 is the maximum value -- i.e., the value of WaitVal
T1_FORMAT = "{int, {enum : 3}, [double:2,3], double}";

T2_NAME = "T2"
# Alternate form of "enum".
T2_FORMAT = \
      "{string, int, <T1:2>, {enum WaitVal, SendVal, ReceiveVal, ListenVal}}"

MSG1        = "message1"
MSG1_FORMAT = "int"

MSG2        = "message2"
MSG2_FORMAT = "string"

QUERY1        = "query1"
QUERY1_FORMAT = T1_NAME

RESPONSE1        = "response1"
RESPONSE1_FORMAT = T2_NAME

MODULE1_NAME = "module1"
MODULE2_NAME = "module2"
MODULE3_NAME = "module3"


