/******************************************************************************
 * PROJECT: New Millennium, DS1
 *          IPC (Interprocess Communication) Package
 *
 * (c) Copyright 2002 Reid Simmons.  All rights reserved.
 *
 * FILE: ipcTest1.java
 *
 * ABSTRACT: Stand-alone test program for Java version of IPC.
 *
 * $Revision: 2.4 $
 * $Date: 2011/08/16 16:00:06 $
 * $Author: reids $
 *
 * Copyright (c) 2008, Carnegie Mellon University
 *     This software is distributed under the terms of the 
 *     Simplified BSD License (see ipc/LICENSE.TXT)
 *
 * REVISION HISTORY
 *
 * $Log: ipcTest1.java,v $
 * Revision 2.4  2011/08/16 16:00:06  reids
 * Adding Python test programs
 *
 * Revision 2.3  2009/09/04 19:09:27  reids
 * IPC Java is now in its own package
 *
 * Revision 2.2  2009/01/12 15:54:58  reids
 * Added BSD Open Source license info
 *
 * Revision 2.1  2002/01/02 21:13:10  reids
 * Added test files for the Java version of IPC, corresponding to the
 *   C and Lisp test files that already existed.
 *
 ****************************************************************/

import ipc.java.*;

public class ipcTest1 {
  private static final String TASK_NAME = "test1";

  private static final String MSG1 = "msg1";
  private static final String MSG2 = "msg2";
  private static final String MSG3 = "msg3";
  private static final String MSG4 = "msg4";
  private static final String MSG5 = "msg5";

  private static final String QUERY_MSG = "query1";

  private static final String QUERY2_MSG    = "query2";
  private static final String RESPONSE2_MSG = "response2";

  private static final String INT_FORMAT    = "int";
  private static final String STRING_FORMAT = "string";

  /*
  private static class sample {
    int i1 = 1;
    String str1 = "";
    double d1 = 0.0;
  }

  private static final String SAMPLE_FORMAT = "{int, string, double}";
  */

  public static class matrix_list {
    public float[][] matrix = null;
    public String matrixName = "";
    public int count = 0;
    public matrix_list next = null;
    public String toString () {
      String str = "{[";
      if (matrix != null)
	for (int i=0; i<matrix.length; i++) {
	    str += "[";
	    for (int j=0; j<matrix[i].length; j++) {
		str += matrix[i][j];
		if (j != matrix[i].length-1) str += ", ";
	    }
	    str += "]";
	    if (i != matrix.length-1) str += ", ";
	}
      str += "]";
      return str +", "+ matrixName +", "+count +", "+next +"}";
    }
  }

  private static final String MATRIX_LIST_FORMAT
      = "{[float:2,2], string, int, *!}";

  private static final String MATRIX_FORMAT = "[float:2,2]";

  public static class msg1Handler implements IPC.HANDLER_TYPE {
      msg1Handler(String theClientData) { clientData = theClientData; }
      public void handle (IPC.MSG_INSTANCE msgInstance, Object callData) {
	  System.out.println("msg1Handler: Receiving message "+ 
			     IPC.msgInstanceName(msgInstance)+" of "+
			     IPC.dataLength(msgInstance)+" bytes");
	  System.out.println("   Java data: "+callData +" ["+clientData+"]");
      }
      String clientData;
  }

  public static class msg2Handler implements IPC.HANDLER_TYPE {
      msg2Handler(String theClientData) { clientData = theClientData; }
      public void handle (IPC.MSG_INSTANCE msgInstance, Object callData) {
	  System.out.println("msg2Handler: Receiving message "+ 
			     IPC.msgInstanceName(msgInstance)+" of "+
			     IPC.dataLength(msgInstance)+" bytes");
	  System.out.println("   Java data: "+callData +" ["+clientData+"]");
      }
      String clientData;
  }

  public static class msg3Handler implements IPC.HANDLER_TYPE {
      public void handle (IPC.MSG_INSTANCE msgInstance, Object callData) {
	System.out.println("msg3Handler: Receiving message "+ 
			   IPC.msgInstanceName(msgInstance)+" of "+
			   IPC.dataLength(msgInstance)+" bytes");
	System.out.println("   Java data: "+callData);
      }
  }

  public static class queryHandler implements IPC.HANDLER_TYPE {
    queryHandler(String theClientData) { clientData = theClientData; }
    public void handle (IPC.MSG_INSTANCE msgInstance, Object callData) {
      String publishStr = "Published";
      String responseStr = "Responded";

      System.out.println("queryHandler: Receiving message "+ 
			 IPC.msgInstanceName(msgInstance)+" of "+
			 IPC.dataLength(msgInstance)+" bytes");
      System.out.println("   Java data: "+callData);
      // Publish this message -- all subscribers get it
      System.out.println("\n  IPC.publishData("+MSG2+", '"+publishStr+"')");
      IPC.publishData(MSG2, publishStr);
      // Respond with this message -- only the query handler gets it
      IPC.respondData(msgInstance, MSG2, responseStr);
    }
    String clientData;
  }

  public static class replyHandler implements IPC.HANDLER_TYPE {
    replyHandler(String theClientData) { clientData = theClientData; }
    public void handle (IPC.MSG_INSTANCE msgInstance, Object callData) {
      System.out.println("replyHandler: Receiving message "+
			 IPC.msgInstanceName(msgInstance)+" ("+callData+") ["+
			 clientData+"]");
      }
    String clientData;
  }

  public static class query2Handler implements IPC.HANDLER_TYPE {
    public void handle (IPC.MSG_INSTANCE msgInstance, Object callData) {
      System.out.println("query2Handler: Receiving message "+
			 IPC.msgInstanceName(msgInstance)+" ("+callData+")");
      float[][] matrix = ((matrix_list)callData).matrix;
      for (int i=0; i<2; i++) for (int j=0; j<2; j++) matrix[i][j]++;
      System.out.println("IPC.respondData("+ RESPONSE2_MSG +", "+
			 matrixString(matrix)+")");
      IPC.respondData(msgInstance, RESPONSE2_MSG, matrix);
    }
    public static String matrixString (float[][] matrix) { 
      String str = "[";
      for (int i=0; i<matrix.length; i++) {
	str += "[";
	for (int j=0; j<matrix[i].length; j++) {
	  str += matrix[i][j];
	  if (j != matrix[i].length-1) str += ", ";
	}
	str += "]";
	if (i != matrix.length-1) str += ", ";
      }
      return str + "]";
    }
  }

  public static class timerHandler implements IPC.TIMER_HANDLER_TYPE {
    timerHandler(String theName, long now) { name = theName; realStart = now; }
    public void handle (long currentTime, long scheduledTime) {
      System.out.println("Timer "+name+": Called at "+
			 currentTime +" ("+ (currentTime - realStart) +
			 "), scheduled at "+ scheduledTime +", delta "+
			 (currentTime - scheduledTime));
    }
    String name;
    long realStart;
  }

  public static class stdinHnd implements IPC.FD_HANDLER_TYPE {
    stdinHnd(String theClientData) { clientData = theClientData; }
    public void handle (int fd) {
      try {
	int in = System.in.read();
	System.out.println("["+clientData+"] Received: "+ (char)in);

	if (in == 'q') {
	  IPC.disconnect();
	  System.exit(-1);
	} else if (in == 'm') {
	  IPC.publishData(MSG2, "Forwarding");
	} else if (in == 'u') {
	  IPC.unsubscribeFD(fd);
	}
	// Read in any extra bytes
	while (System.in.available() > 0) System.in.read();
      } catch (Exception e) { e.printStackTrace(); }
    }
    String clientData;
  }

  public static class handlerChangeHandler implements IPC.CHANGE_HANDLE_TYPE {
      public void handle (String msgName, int numHandlers) {
	System.out.println("There are now "+ numHandlers 
			   +" handlers for message "+ msgName);
      }
  }

  public static class connectionHandler implements IPC.CONNECT_HANDLE_TYPE {
      connectionHandler(boolean connectNotify) { isConnect = connectNotify; }
      public void handle (String moduleName) {
	System.out.println(moduleName +" just "+
			   (isConnect ? "connected" : "disconnected")
			   +" from the IPC network");
      }
      boolean isConnect;
  }

  /****************************************************************
   *                TESTS OF THE BASIC IPC INTERFACE
   ****************************************************************/
  public static void main (String args[]) throws Exception {
    // Connect to the central server
    System.out.println("\nIPC.connect("+ TASK_NAME +")");
    IPC.connect(TASK_NAME);

    // Just for kicks -- make sure it works
    IPC.setContext(IPC.getContext());

    // Default is to exit on error; Override default, because some of the
    // tests in this file explicitly induce errors.
    System.out.println("\nIPC.setVerbosity(IPC.Print_Errors)");
    IPC.setVerbosity(IPC.IPC_Print_Errors);

    /* Test out the timers */
    long now = IPC.timeInMillis();
    System.out.println("\nIPC_addTimer(1000, TRIGGER_FOREVER, "+
		       " new timerHandler(\"timer1\", "+now+"))");
    IPC.addTimer(1000, IPC.TRIGGER_FOREVER, new timerHandler("timer1", now));

    // Does not make sense to define a fixed-length Java message 
    // Define a variable length message (simple format string)
    System.out.println("\nIPC.defineMsg("+ MSG1 +", "+ STRING_FORMAT +")");
    IPC.defineMsg(MSG1, STRING_FORMAT);
    // Define a variable length message (simple format string)
    System.out.println("\nIPC.defineMsg("+ MSG2 +", "+ STRING_FORMAT +")");
    IPC.defineMsg(MSG2, STRING_FORMAT);

    System.out.println("\nIPC.isMsgDefined("+ MSG1 +") => "+
		       IPC.isMsgDefined(MSG1));
    System.out.println("\nIPC.isMsgDefined("+ MSG3 +") => "+
		       IPC.isMsgDefined(MSG3));

    // Subscribe to the first message, with client data
    System.out.println("\nIPC.subscribeData("+ MSG1 
		       +", new msg1Handler(client1a))");
    msg1Handler msgHandler1 = new msg1Handler("client1a");
    IPC.subscribeData(MSG1, msgHandler1, String.class);

    // Subscribe to the second message, with client data
    System.out.println("\nIPC.subscribeData("+ MSG2
		       +", new msg1Handler(client2a))");
    IPC.subscribeData(MSG2, new msg1Handler("client2a"), String.class);

    // Publish the first message and listen for it (in this simple 
    //   example program, both publisher and subscriber are in the same process
    String msgData = "abcde";
    System.out.println("\nIPC.publishData("+ MSG1 +", "+ msgData +")");
    if (IPC.publishData(MSG1, msgData) == IPC.IPC_OK)
	IPC.listenClear(1000);

    msgData = "abcde";
    System.out.println("\nIPC.publishData("+ MSG2 +", "+ msgData +")");
    if (IPC.publishData(MSG2, msgData) == IPC.IPC_OK)
	IPC.listenClear(1000);

    // Get notified when handlers subscribe/unsubscribe to "msg2"
    System.out.println("\n"+ IPC.numHandlers(MSG2) 
		       +" handlers currently subscribed to "+ MSG2);
    System.out.println("\nIPC.subscribeHandlerChange("+MSG2
		       +", new handlerChangeHandler())");
    IPC.subscribeHandlerChange(MSG2, new handlerChangeHandler());

    // Subscribe a second message handler for "msg2"
    System.out.println("\nIPC.subscribeData("+ MSG2
		       +", new msg2Handler(client2b))");
    IPC.subscribeData(MSG2, new msg2Handler("client2b"), String.class);
    // If doing direct broadcasts, need to listen to get the direct info update
    IPC.listen(250);

    // Publish the message -- receive two messages (one for msg1Handler, one
    //   for msg2Handler).

    msgData = "abcdefg";
    System.out.println("\nIPC.publishData("+ MSG2 +", "+ msgData +")");
    if (IPC.publishData(MSG2, msgData) == IPC.IPC_OK) {
      // Make sure all the subscribers get invoked before continuing on
      //  (keep listening until a second has passed without any msgs)
      while (IPC.listen(1000) != IPC.IPC_Timeout) {}
    }

    // Remove this subscription.  You can unsubscribe either by passing
    //   the handler class, or the handler object itself (see below).
    System.out.println("\nIPC.unsubscribe("+ MSG2 +", msg2Handler)");
    IPC.unsubscribe(MSG2, msg2Handler.class);
    // If doing direct broadcasts, need to listen to get the direct info update
    IPC.listen(250);

    // No longer get notified when handlers are added/removed
    System.out.println("\nIPC.unsubscribeHandlerChange("+ MSG2
		       +", handlerChangeHandler.class)");
    IPC.unsubscribeHandlerChange(MSG2, handlerChangeHandler.class);
    System.out.println("  IPC.subscribeData("+ MSG2
		       +", new msg2Handler(client2c))");
    IPC.subscribeData(MSG2, new msg2Handler("client2b"), String.class);
    System.out.println("  IPC.unsubscribe("+ MSG2 +", msg2Handler)");
    IPC.unsubscribe(MSG2, msg2Handler.class);

    // Publish the message -- receive one message (for msg1Handler)
    msgData = "abcdefg";
    System.out.println("\nIPC.publishData("+ MSG2 +", "+ msgData +")");
    if (IPC.publishData(MSG2, msgData) == IPC.IPC_OK) { IPC.listen(1000); }

    // Java version of IPC differs from C version in how it handles client data
    // In Java, the client data is part of the handler class.
    System.out.println("\nReplacing client data for msg1Handler of "+
		       MSG1 +" with 'client1b'");
    msgHandler1.clientData = "client1b";

    // Receive one message (for msg1Handler), but now with new client data.
    msgData = "abcde";
    System.out.println("\nIPC.publishData("+ MSG1 +", "+ msgData +")");
    if (IPC.publishData(MSG1, msgData) == IPC.IPC_OK) { IPC.listen(1000); }

    // Remove subscription to "msg1"
    System.out.println("\nIPC.unsubscribe("+ MSG1 +", msg1Handler)");
    IPC.unsubscribe(MSG1, msgHandler1);
    // If doing direct broadcasts, need to listen to get the direct info update
    IPC.listen(250);

    // Receive no messages -- IPC.listenClear times out
    System.out.println("\nIPC.publishData("+ MSG1 +", "+ msgData +")");
    if (IPC.publishData(MSG1, msgData) == IPC.IPC_OK) { 
	if (IPC.listen(1000) == IPC.IPC_Timeout) {
	    System.out.println("Timed out");
	}
    }

    ////////////////////////////////////////////////////////////////
    //                TESTS OF THE QUERY/RESPONSE FUNCTIONS
    ////////////////////////////////////////////////////////////////

    // The handler of QUERY_MSG does two things: It *publishes* a message of
    //  type MSG2, and it *responds* to the query with a message of type MSG2.
    //  The published message gets handled only by the subscriber (msg1Handler),
    //  and the response gets handled only be replyHandler, since a response
    //  is a directed message.
    // NOTE: It is perfectly OK to subscribe to a message before it is defined!
    System.out.println("\nIPC.subscribeData("+ QUERY_MSG
		       +", new queryHandler('qtest'))");
    IPC.subscribeData(QUERY_MSG, new queryHandler("qtest"), String.class);
    System.out.println("\nIPC.defineMsg("+ QUERY_MSG +", "+ STRING_FORMAT +")");
    IPC.defineMsg(QUERY_MSG, STRING_FORMAT);

    // This call allows IPC to send the process 2 messages at a time, rather 
    // than queueing them in the central server.
    // This is needed in this example program because the sender and receiver
    // of the query are the same process.  If this is taken out, the only 
    // difference is that the message that is published in queryHandler arrives
    // *after* the message responded to (even though it is sent first).  This 
    // function should not be needed when we switch to using point-to-point 
    // communications (rather than sending via the central server).
    IPC.setCapacity(2);

    System.out.println("\nIPC.queryNotifyData("+ QUERY_MSG +", 'abcdef', "+
		       "new replyHandler('Notification')");
    IPC.queryNotifyData(QUERY_MSG, "abcdef", new replyHandler("Notification"),
			String.class);
    // Make sure all the messages spawned by this query get handled before
    //   continuing (keep listening until a second has passed without any msgs)
    while (IPC.listen(1000) != IPC.IPC_Timeout){};

    // This essentially does the same thing as queryNotifyData above, except
    //  it is blocking, and sets the replyHandle to be the data responded to.
    //  Don't need to listen, since that is done within queryResponse, but
    //  could be dangerous to wait forever (if the response never comes ...)
    msgData = "abcdef";
    System.out.println("\nIPC.queryResponseData("+ QUERY_MSG +", "+ msgData
		       +", String.class, IPC.IPC_WAIT_FOREVER)");
    Object response = IPC.queryResponseData(QUERY_MSG, msgData, String.class,
					    IPC.IPC_WAIT_FOREVER);
    if (response != null) {
	System.out.println("Blocking Response: "+ response);
    }

    // This one should time out before the response arrives
    System.out.println("\nIPC.queryResponseData("+ QUERY_MSG +", "+ msgData
		       +", String.class, 0)");
    response = IPC.queryResponseData(QUERY_MSG, msgData, String.class, 0);
    if (response != null) {
	System.out.println("Blocking Response: "+ response);
    } else {
	// NOTE: Since the function call times out before handling messages,
	// (a) The *response* to the query is lost (for good)
	// (b) The message *published* in queryHandler is waiting for the
	//     next time the module listens for messages (which actually 
	//     occurs in IPC.msgFormatter, below).
	System.out.println("queryResponse timed out (response is null)");
    }

    IPC.listenClear(1000);

    ////////////////////////////////////////////////////////////////
    //                TESTS OF THE MARSHALLING FUNCTIONS
    ////////////////////////////////////////////////////////////////

    // Define a variable-length message whose format is simply an integer
    System.out.println("\nIPC.defineMsg("+ MSG3 +", "+ INT_FORMAT+")");
    IPC.defineMsg(MSG3, INT_FORMAT);
    // Define a variable-length message whose format is a string
    System.out.println("\nIPC.defineMsg("+ MSG4  +", "+ STRING_FORMAT+")");
    IPC.defineMsg(MSG4, STRING_FORMAT);
    // Define a variable-length message whose format is a complex structure
    System.out.println("\nIPC.defineMsg("+ MSG5 +", "+  MATRIX_LIST_FORMAT+")");
    IPC.defineMsg(MSG5, MATRIX_LIST_FORMAT);

    // Subscribe to each of the above messages, all using the same handler
    //   Differentiated by the dataClass argument...
    System.out.println("\nIPC.subscribeData("+ MSG3 +", new msg3Handler())");
    IPC.subscribeData(MSG3, new msg3Handler(), int.class);
    System.out.println("\nIPC.subscribeData("+ MSG4 +", new msg3Handler())");
    IPC.subscribeData(MSG4, new msg3Handler(), String.class);
    System.out.println("\nIPC.subscribeData("+ MSG5 +", new msg3Handler())");
    IPC.subscribeData(MSG5, new msg3Handler(), matrix_list.class);

    // Publish integer data
    int ivalue = 42;
    System.out.println("\nIPC.publishData("+ MSG3  +", "+ ivalue+")");
    IPC.publishData(MSG3, ivalue);
    IPC.listenClear(1000);

    // Publish string data
    msgData = "Hello, world";
    System.out.println("\nIPC.publishData("+ MSG4  +", "+ msgData+")");
    IPC.publishData(MSG4, msgData);
    IPC.listenClear(1000);

    // Set up a sample matrix_list structure
    matrix_list m1 = new matrix_list();
    m1.matrix = new float[][] {{0, 1},{1, 2}};
    m1.matrixName = "TheFirst";
    m1.count = 1;
    m1.next = new matrix_list();
    m1.next.matrix = new float[][] {{1, 2},{2, 3}};
    m1.next.matrixName = "TheSecond";
    m1.next.count = 2;
    m1.next.next = new matrix_list();
    m1.next.next.matrix = new float[][] {{2, 3},{3, 4}};
    m1.next.next.matrixName = "TheThird";
    m1.next.next.count = 3;
    m1.next.next.next = null;

    System.out.println("\nIPC.publishData("+ MSG5  +", "+ m1+")");
    IPC.publishData(MSG5, m1);
    IPC.listenClear(1000);

    // Use of IPC.queryResponseData and IPC.respondData -- 
    //   Send out a message with a matrix_list format  The response is a
    //   message with the first matrix, but each element incremented by one.

    // Define the "query" message
    System.out.println("\nIPC.defineMsg("+
		       QUERY2_MSG +", "+ MATRIX_LIST_FORMAT+")");
    IPC.defineMsg(QUERY2_MSG, MATRIX_LIST_FORMAT);
    // Define the "response" message
    System.out.println("\nIPC.defineMsg("+
		       RESPONSE2_MSG +", "+  MATRIX_FORMAT+")");
    IPC.defineMsg(RESPONSE2_MSG, MATRIX_FORMAT);

    // Subscribe to query message with automatic unmarshalling
    System.out.println("\nIPC.subscribeData("+ 
		       QUERY2_MSG  +", new query2Handler())");
    IPC.subscribeData(QUERY2_MSG, new query2Handler(), matrix_list.class);

    // Set up a sample MATRIX_LIST structure
    m1 = new matrix_list();
    m1.matrix = new float[][] {{0, 1},{1, 2}};
    m1.matrixName = "TheFirst";
    m1.count = 1;
    m1.next = null;

    // IPC.queryResponseData both marsalls and sends the data structure
    System.out.println("\nIPC.queryResponseData("+ 
		       QUERY2_MSG  +", "+ m1  +", IPC.IPC_WAIT_FOREVER)");
    Object matrixReply = IPC.queryResponseData(QUERY2_MSG, m1,
					       float [][].class,
					       IPC.IPC_WAIT_FOREVER);
    if (matrixReply != null)
      System.out.println("response data: "+ 
			 query2Handler.matrixString((float[][])matrixReply));
    else
      System.out.println("IPC.queryResponseData failed");

    /* Subscribe a handler for tty input.  Now, typing at the terminal will
       echo the input.  Typing "q" will quit the program; typing "m" will
       send a message; typing "u" will unsubscribe the handler (the program
       will no longer listen to input). */
    System.out.println("\nIPC.subscribeFD(0, new stdinHnd(FD1))");
    IPC.subscribeFD(0, new stdinHnd("FD1"));
    System.out.println("\nEntering dispatch loop (terminal input is echoed,"+
		       " type 'q' to quit,");
    System.out.println("  'm' to send a message,"
		       +" 'u' to stop listening to stdin).");
    IPC.dispatch();

    // Shut down gracefully
    IPC.disconnect();
  }
}
