##############################################################################
 # PROJECT: IPC (Interprocess Communication) Package
 #
 # (c) Copyright 2011 Reid Simmons.  All rights reserved.
 #
 # FILE: ipcTest1.py
 #
 # ABSTRACT: Stand-alone test program for Python version of IPC.
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
 # $Log: ipcTest1.py,v $
 # Revision 2.1  2011/08/16 16:00:07  reids
 # Adding Python test programs
 #
 ################################################################/

from IPC import *
import sys

TASK_NAME = "test1"

MSG1 = "msg1"
MSG2 = "msg2"
MSG3 = "msg3"
MSG4 = "msg4"
MSG5 = "msg5"

QUERY_MSG = "query1"

QUERY2_MSG =    "query2"
RESPONSE2_MSG = "response2"

INT_FORMAT = "int"
STRING_FORMAT = "string"

class SAMPLE_TYPE(IPCdata) :
    _fields = ('i1', 'str1', 'd1')

SAMPLE_FORMAT = "{int, string, double}"

class MATRIX_LIST_TYPE(IPCdata) :
    _fields = ('matrix', 'matrixName', 'count', 'next')

MATRIX_LIST_FORMAT = "{[float:2,2], string, int, *!}"
MATRIX_FORMAT =      "[float:2,2]"

def msg1Handler (msgRef, callData, clientData) :
  (obj, ret) = IPC_unmarshall(IPC_msgInstanceFormatter(msgRef), callData)
  print "msg1Handler: Receiving message %s of %d bytes (%s/%s) [%s]" % \
        (IPC_msgInstanceName(msgRef), IPC_dataLength(msgRef),
         obj, callData, clientData)
  IPC_freeByteArray(callData)

def msg2Handler (msgRef, callData, clientData) :
  print "msg2Handler: Receiving message %s of %d bytes (%s) [%s]" % \
        (IPC_msgInstanceName(msgRef), IPC_dataLength(msgRef),
	 callData, clientData)

# Unmarshall the byte array and print it out. 
def msg3Handler (msgRef, callData, clientData) :
  print "msg3Handler: Receiving message %s: " % IPC_msgInstanceName(msgRef),
  # Get the formatter for this message instance
  formatter = IPC_msgInstanceFormatter(msgRef)
  (obj, ret) = IPC_unmarshall(formatter, callData)
  IPC_printData(formatter, sys.stdout, obj)
  # Free up malloc'd data
  IPC_freeByteArray(callData)

def queryHandler (msgRef, callData, clientData) :
  publishStr = "Published"
  responseStr = "Responded"

  print "queryHandler: Receiving message %s of %d bytes (%s) [%s]" % \
        (IPC_msgInstanceName(msgRef), IPC_dataLength(msgRef),
	 callData, clientData)
  # Publish this message -- all subscribers get it 
  print "\n  IPC_publishData(%s, %s)" % (MSG2, publishStr)
  IPC_publishData(MSG2, publishStr)
  # Respond with this message -- only the query handler gets it
  print "\n  IPC_respondData(msgRef, %s, %s)" % (MSG2, responseStr)
  IPC_respondData(msgRef, MSG2, responseStr)

# Handles the response to QUERY_MSG messages
def replyHandler (msgRef, callData, clientData) :
  print "replyHandler: Receiving message %s (%s) [%s]" % \
        (IPC_msgInstanceName(msgRef), callData, clientData)

# Incoming message has a matrix_list format;  The response is a
#   message with the first matrix, but each element incremented by one.
def query2Handler (msgRef, matrixList, clientData) :
  print "query2Handler: Receiving message %s [%s]" % \
        (IPC_msgInstanceName(msgRef), clientData)

  formatter = IPC_msgInstanceFormatter(msgRef)
  IPC_printData(formatter, sys.stdout, matrixList)

  for i in range(0, 2) :
    for j in range(0, 2) :
      matrixList.matrix[i][j] = matrixList.matrix[i][j]+1

  print "\n  IPC_respondData(msgRef, %s, matrixList.matrix)" % RESPONSE2_MSG
  IPC_respondData(msgRef, RESPONSE2_MSG, matrixList.matrix)

def timerHandler (msg, currentTime, scheduledTime) :
  print "Timer %s: Called at %ld, scheduled at %ld, delta %ld" % \
        (msg, currentTime, scheduledTime, (currentTime - scheduledTime))

def handlerChangeHandler (msgName, numHandlers, clientData) :
  print "There are now %d handlers for message %s" % (numHandlers, msgName)

# Echo the input.  In addition,
#  typing "q" will quit the program; 
#  typing "m" will send another message; 
#  typing "u" will unsubscribe the handler (the program will no
#    longer listen to input)
done = False

def stdinHnd (fd, clientData) :
  global done
  input = sys.stdin.readline()

  print "stdinHnd [%s]: Received %s" % (clientData, input)

  if (len(input) == 2) : # Only one character and the newline
    if (input[0] == 'q') :
      IPC_disconnect()
      done = True
    elif (input[0] == 'm') :
      msg = "Forwarding"
      print "\n  IPC_publishData(%s, %s)\n" % (MSG2, msg)
      IPC_publishData(MSG2, msg)
    elif (input[0] == 'u') :
      IPC_unsubscribeFD(fd, stdinHnd)

def main () :
  global done
  done = False

  # Actually send one more byte (end-of-string)
  FIVE_BYTE_MSG_LEN  = 6
  SIX_BYTE_MSG_LEN   = 7
  EIGHT_BYTE_MSG_LEN = 9

  fiveBytes = "abcde"
  sixBytes = "abcdef"
  eightBytes = "abcdefgh"

  #################################################################
  #                TESTS OF THE BASIC IPC INTERFACE
  #################################################################/

  # Connect to the central server
  print "\nIPC_connect(%s)\n", TASK_NAME
  IPC_connect(TASK_NAME)

  # Default is to exit on error; Override default, because some of the
  #   tests in this file explicitly induce errors. 
  print "\nIPC_setVerbosity(IPC_Print_Errors)"
  IPC_setVerbosity(IPC_Print_Errors)

  # Test out the timers 
  print "\nIPC_addTimer(1000, TRIGGER_FOREVER, timerHandler, \"timer1\")"
  IPC_addTimer(1000, TRIGGER_FOREVER, timerHandler,"timer1")

  # Although it would be nice to be consistent with ipcTest1.c,
  # not much utility in defining a fixed-length Python message
  # Define a variable length message (simple format string)
  print "\nIPC_defineMsg(%s, IPC_VARIABLE_LENGTH, %s)" % (MSG1, STRING_FORMAT)
  IPC_defineMsg(MSG1, IPC_VARIABLE_LENGTH, STRING_FORMAT)
  # Define a variable length message (simple format string)
  print "\nIPC_defineMsg(%s, IPC_VARIABLE_LENGTH, %s)" % (MSG2, STRING_FORMAT)
  IPC_defineMsg(MSG2, IPC_VARIABLE_LENGTH, STRING_FORMAT)

  print "\nIPC_isMsgDefined(%s) => %s" % (MSG1, IPC_isMsgDefined(MSG1))
  print "\nIPC_isMsgDefined(%s) => %s"  % (MSG3, IPC_isMsgDefined(MSG3))

  # Subscribe to the first message, with client data 
  print "\nIPC_subscribe(%s, msg1Handler, %s)" % (MSG1, "client1a")
  IPC_subscribe(MSG1, msg1Handler, "client1a")
  # Subscribe to the second message, with automatic unmarshalling and
  # passing along client data
  print "\nIPC_subscribeData(%s, msg1Handler, %s)" % (MSG2, "client2a")
  IPC_subscribeData(MSG2, msg2Handler, "client2a")

  IPC_listenClear(1000) # Give chance for central to update information

  # Publish the first message and listen for it (in this simple 
  #   example program, both publisher and subscriber are in the same process 
  print "\nIPC_publishVC(%s, %s)" % (MSG1, fiveBytes)
  vc = IPC_VARCONTENT_TYPE()
  IPC_marshall(IPC_parseFormat(STRING_FORMAT), fiveBytes, vc)
  if (IPC_publishVC(MSG1, vc) == IPC_OK) :
    IPC_listenClear(1000)
  else : print "EPIC FAIL"

  # Publish the second message
  print "\nIPC_publishData(%s, %s)" % (MSG2, eightBytes)
  if (IPC_publishData(MSG2, eightBytes) == IPC_OK) :
    IPC_listenClear(1000)

  # Get notified when handlers subscribe/unsubscribe to "msg2" 
  print "\n%d handlers currently subscribed to %s" % \
        (IPC_numHandlers(MSG2), MSG2)
  print '\nIPC_subscribeHandlerChange("MSG2", handlerChangeHandler, None)'
  IPC_subscribeHandlerChange(MSG2, handlerChangeHandler, None)

  # Subscribe a second message handler for "msg2" 
  print "\nIPC_subscribe(%s, msg2Handler, %s)" % (MSG2, "client2b")
  IPC_subscribe(MSG2, msg2Handler, "client2b")
  # If doing direct broadcasts, need to listen to get the direct info update 
  IPC_listen(250)

  # Publish the message -- receive two messages (one for msg1Handler, 
  #   one for msg2Handler). 
  # Send one extra byte -- for end-of-string 
  print "\nIPC_publishData(%s, %s)" % (MSG2, eightBytes)
  if (IPC_publishData(MSG2, eightBytes) == IPC_OK) :
    # Make sure all the subscribers get invoked before continuing on
    #   (keep listening until a second has passed without any msgs) 
    while (IPC_listen(1000) != IPC_Timeout) : pass

  # Remove this subscription 
  print "\nIPC_unsubscribe(%s, msg2Handler)" % MSG2
  IPC_unsubscribe(MSG2, msg2Handler)
  # If doing direct broadcasts, need to listen to get the direct info update 
  IPC_listen(250)

  # No longer get notified when handlers are added/removed 
  print '\nIPC_unsubscribeHandlerChange("MSG2", handlerChangeHandler)'
  IPC_unsubscribeHandlerChange(MSG2, handlerChangeHandler)
  print "  IPC_subscribe(%s, msg2Handler, %s)" % (MSG2, "client2c")
  IPC_subscribe(MSG2, msg2Handler, "client2c")
  print "  IPC_unsubscribe(%s, msg2Handler)" % MSG2
  IPC_unsubscribe(MSG2, msg2Handler)

  # Publish the message -- receive one message (for msg1Handler)
  print "\nIPC_publishData(%s, %s)" % (MSG2, eightBytes)
  if (IPC_publishData(MSG2, eightBytes) == IPC_OK) :
    IPC_listenClear(1000)

  # Subscription of the same message handler #replaces* the old client data
  print "\nIPC_subscribe(%s, msg1Handler, %s)" % (MSG1, "client1b")
  IPC_subscribe(MSG1, msg1Handler, "client1b")
  # Receive one message (for msg1Handler), but now with new client data.
  print "\nIPC_publishData(%s, %s)" % (MSG1, fiveBytes)
  if (IPC_publishData(MSG1, fiveBytes) == IPC_OK) : IPC_listenClear(1000)

  # Remove subscription to "msg1" 
  print "\nIPC_unsubscribe(%s, msg1Handler)" % MSG1
  IPC_unsubscribe(MSG1, msg1Handler)
  # If doing direct broadcasts, need to listen to get the direct info update 
  IPC_listen(250)

  # Receive no messages -- IPC_listenClear times out 
  print "\nIPC_publishData(%s, %s)" % (MSG1, fiveBytes)
  if (IPC_publishData(MSG1, fiveBytes) == IPC_OK) :
    if (IPC_listenClear(0) == IPC_Timeout) : print "Timed out\n"

  ################################################################
  #                TESTS OF THE QUERY/RESPONSE FUNCTIONS
  ###############################################################

  # The handler of QUERY_MSG does two things: It *publishes* a message of
  #  type MSG2, and it *responds* to the query with a message of type MSG2.
  #  The published message gets handled only by the subscriber (msg1Handler),
  #  and the response gets handled only be replyHandler, since a response
  #  is a directed message. 
  # NOTE: It is perfectly OK to subscribe to a message before it is defined! 
  print "\nIPC_subscribeData(%s, queryHandler, %s)" % (QUERY_MSG, "qtest")
  IPC_subscribeData(QUERY_MSG, queryHandler, "qtest")
  print "\nIPC_defineMsg(%s, IPC_VARIABLE_LENGTH, %s)" % \
        (QUERY_MSG, STRING_FORMAT)
  IPC_defineMsg(QUERY_MSG, IPC_VARIABLE_LENGTH, STRING_FORMAT)

  # This call allows IPC to send the process 2 messages at a time, rather than
  #  queueing them in the central server.
  #  This is needed in this example program because the sender and receiver of
  #  the query are the same process.  If this is taken out, the only difference
  #  is that the message that is published in queryHandler arrives *after* the
  #  message responded to (even though it is sent first).  This function should
  #  not be needed when we switch to using point-to-point communications
  #  (rather than sending via the central server). 
  IPC_setCapacity(2)

  # Send one extra byte -- for end-of-string 
  print "\nIPC_queryNotifyData(%s, %s, replyHandler, %s)" % \
        (QUERY_MSG, sixBytes, "Notification")
  IPC_queryNotifyData(QUERY_MSG, sixBytes, replyHandler, "Notification")
  # Make sure all the messages spawned by this query get handled before
  #   continuing (keep listening until a second has passed without any msgs) 
  while (IPC_listen(1000) != IPC_Timeout) : pass

  # This essentially does the same thing as IPC_queryNotify above, except
  #  it is blocking, and sets the reply to be the data responded to.
  #  Don't need to listen, since that is done within queryResponse, but
  #  could be dangerous to wait forever (if the response never comes ...) 
  print "\nIPC_queryResponseData(%s, %s, replyHandle, IPC_WAIT_FOREVER)" % \
        (QUERY_MSG, sixBytes)
  (reply, ret) = IPC_queryResponseData(QUERY_MSG, sixBytes, IPC_WAIT_FOREVER)
  if (ret == IPC_OK) :
    print "Blocking Response: %s" % reply

  # This one should time out before the response arrives 
  print "\nIPC_queryResponseData(%s, %s, replyHandle, %d)" % \
        (QUERY_MSG, sixBytes, 0)
  (reply, ret) = IPC_queryResponseData(QUERY_MSG, sixBytes, 0)
  if (ret == IPC_OK) :
    print "Blocking Response: %s" % reply
  else :
    # NOTE: Since the function call times out before handling messages,
    # (a) The *response* to the query is lost (for good)
    # (b) The message *published* in queryHandler is waiting for the next
    #     time the module listens for messages (which actually occurs
    #     in IPC_msgFormatter, below).
    print "queryResponse timed out (replyHandle: %s)" % reply

  ################################################################
  #                TESTS OF THE MARSHALLING FUNCTIONS
  ###############################################################

  # Test the marshalling/unmarshalling functions, independently of
  #  sending/receiving messages
  varcontent = IPC_VARCONTENT_TYPE()
  sample = SAMPLE_TYPE()

  sample.i1 = 666; sample.str1 = "hello, world"; sample.d1 = 3.14159

  print "\nIPC_marshall(...)"
  IPC_marshall(IPC_parseFormat(SAMPLE_FORMAT), sample, varcontent)
  print "Marshall of 'sample' [length: %d]" % varcontent.length
    
  print "\nIPC_unmarshall(...)"
  (sample2, ret) = IPC_unmarshall(IPC_parseFormat(SAMPLE_FORMAT),
                                  varcontent.content, oclass=SAMPLE_TYPE)
  print "Orig: <%d, %s, %f>\nCopy: <%d, %s, %f>" % \
        (sample.i1, sample.str1, sample.d1,
         sample2.i1, sample2.str1, sample2.d1)
  IPC_freeByteArray(varcontent.content)

  # Define a variable-length message whose format is simply an integer 
  print "\nIPC_defineMsg(%s, IPC_VARIABLE_LENGTH, %s)" % (MSG3, INT_FORMAT)
  IPC_defineMsg(MSG3, IPC_VARIABLE_LENGTH, INT_FORMAT)
  # Define a variable-length message whose format is a string 
  print "\nIPC_defineMsg(%s, IPC_VARIABLE_LENGTH, %s)" % (MSG4, STRING_FORMAT)
  IPC_defineMsg(MSG4, IPC_VARIABLE_LENGTH, STRING_FORMAT)
  # Define a variable-length message whose format is a complex structure 
  print "\nIPC_defineMsg(%s, IPC_VARIABLE_LENGTH, %s)" % \
        (MSG5, MATRIX_LIST_FORMAT)
  IPC_defineMsg(MSG5, IPC_VARIABLE_LENGTH, MATRIX_LIST_FORMAT)

  # Subscribe to each of the above messages, all using the same handler 
  print "\nIPC_subscribe(%s, msg3Handler, None)" % MSG3
  IPC_subscribe(MSG3, msg3Handler, None)
  print "\nIPC_subscribe(%s, msg3Handler, None)" % MSG4
  IPC_subscribe(MSG4, msg3Handler, None)
  print "\nIPC_subscribe(%s, msg3Handler, None)" % MSG5
  IPC_subscribe(MSG5, msg3Handler, None)

  IPC_listenClear(1000) # Give chance for central to update information

  # Marshall the integer into a byte array (takes byte-order into account) 
  print "\nIPC_marshall(...)"
  IPC_marshall(IPC_msgFormatter(MSG3), 42, varcontent)
  # Publish the marshalled byte array (message handler prints the data) 
  print "\nIPC_publishVC(%s, varcontent[%d])" % (MSG3, varcontent.length)
  IPC_publishVC(MSG3, varcontent)
  IPC_freeByteArray(varcontent.content)
  IPC_listenClear(1000)

  # Marshall the string into a byte array. NOTE: Need to pass a *pointer*
  #  to the string! 
  # It's much better (safer) to use IPC_msgFormatter, but this is included
  #  just to illustrate the use of IPC_parseFormat 
  print "\nIPC_marshall(...)"
  IPC_marshall(IPC_parseFormat(STRING_FORMAT), "Hello, world", varcontent)
  # Publish the marshalled byte array (message handler prints the data) 
  print "\nIPC_publishVC(%s, varcontent[%d])" % (MSG4, varcontent.length)
  IPC_publishVC(MSG4, varcontent)
  IPC_freeByteArray(varcontent.content)
  IPC_listenClear(1000)

  # Set up a sample MATRIX_LIST structure 
  m1 = MATRIX_LIST_TYPE()
  m2 = MATRIX_LIST_TYPE()
  m3 = MATRIX_LIST_TYPE()
  k = 0
  m1.matrix = [None]*2
  for i in range(0,2) :
    m1.matrix[i] = [None]*2
    for j in range(0,2) : m1.matrix[i][j] = (i+j+k)
  m1.matrixName = "TheFirst"; m1.count = k; m1.next = m2

  k = k + 1
  m2.matrix = [None]*2
  for i in range(0,2) :
    m2.matrix[i] = [None]*2
    for j in range(0,2) : m2.matrix[i][j] = (i+j+k)
  m2.matrixName = "TheSecond"; m2.count = k; m2.next = m3

  k = k + 1
  m3.matrix = [None]*2
  for i in range(0,2) :
    m3.matrix[i] = [None]*2
    for j in range(0,2) : m3.matrix[i][j] = (i+j+k)
  m3.matrixName = "TheThird"; m3.count = k; m3.next = None

  # IPC_publishData both marsalls and publishes the data structure 
  print "\nIPC_publishData(%s, m1)" % MSG5
  IPC_publishData(MSG5, m1)
  IPC_listenClear(1000)

  # Use of IPC_queryResponseData and IPC_respondData -- 
  # Send out a message with a matrix_list format;  The response is a
  # message with the first matrix, but each element incremented by one. 

  # Define the "query" message 
  print "\nIPC_defineMsg(%s, IPC_VARIABLE_LENGTH, %s)" % \
        (QUERY2_MSG, MATRIX_LIST_FORMAT)
  IPC_defineMsg(QUERY2_MSG, IPC_VARIABLE_LENGTH, MATRIX_LIST_FORMAT)
  # Define the "response" message 
  print "\nIPC_defineMsg(%s, IPC_VARIABLE_LENGTH, %s)" % \
        (RESPONSE2_MSG, MATRIX_FORMAT)
  IPC_defineMsg(RESPONSE2_MSG, IPC_VARIABLE_LENGTH, MATRIX_FORMAT)

  # Subscribe to query message with automatic unmarshalling 
  print "\nIPC_subscribeData(%s, query2Handler, None)" % QUERY2_MSG
  IPC_subscribeData(QUERY2_MSG, query2Handler, None, MATRIX_LIST_TYPE)

  # Set up a sample MATRIX_LIST structure 
  k = 0
  m1.matrix = [None]*2
  for i in range(0,2) :
    m1.matrix[i] = [None]*2
    for j in range(0,2) : m1.matrix[i][j] = (i+j+k)
  m1.matrixName = "TheFirst"; m1.count = k; m1.next = None

  # IPC_queryResponseData both marsalls and sends the data structure 
  print "\nIPC_queryResponseData(%s, m1, matrixPtr, IPC_WAIT_FOREVER)" % \
        QUERY2_MSG
  (matrix, ret) = IPC_queryResponseData(QUERY2_MSG, m1, IPC_WAIT_FOREVER)
  if (ret == IPC_OK) :
    IPC_printData(IPC_msgFormatter(RESPONSE2_MSG), sys.stdout, matrix)
  else :
    print "IPC_queryResponseData failed"

  # Subscribe a handler for tty input.  Now, typing at the terminal will
  # echo the input.  Typing "q" will quit the program; typing "m" will
  # send a message; typing "u" will unsubscribe the handler (the program
  # will no longer listen to input). 
  print "\nIPC_subscribeFD(%d, stdinHnd, %s)" % (sys.stdin.fileno(), "FD1")
  IPC_subscribeFD(sys.stdin.fileno(), stdinHnd, "FD1")
  print "\nEntering dispatch loop (terminal input is echoed, type 'q' to quit,"
  print "  'm' to send a message, 'u' to stop listening to stdin)."
  while (not done) : IPC_listen(1000)

  # If ever reaches here, shut down gracefully 
  IPC_disconnect()
