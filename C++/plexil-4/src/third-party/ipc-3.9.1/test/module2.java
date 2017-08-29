/******************************************************************************
 * PROJECT: New Millennium, DS1
 *          IPC (Interprocess Communication) Package
 *
 * (c) Copyright 2002 Reid Simmons.  All rights reserved.
 *
 * FILE: module2.java
 *
 * ABSTRACT: Test program for IPC.
 *             Publishes: MSG2
 *             Subscribes to: MSG1, QUERY1
 *             Responds with: RESPONSE1
 *             Behavior: Listens for MSG1 and prints out message data.
 *                       When QUERY1 is received, publishes MSG1 and
 *                       responds to the query with RESPONSE1.
 *                       Exits when 'q' is typed at terminal.
 *                       Should be run in conjunction with module1
 *
 * $Revision: 2.3 $
 * $Date: 2009/09/04 19:09:27 $
 * $Author: reids $
 *
 * Copyright (c) 2008, Carnegie Mellon University
 *     This software is distributed under the terms of the 
 *     Simplified BSD License (see ipc/LICENSE.TXT)
 *
 * REVISION HISTORY
 *
 * $Log: module2.java,v $
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

public class module2 extends module {
  private static class msg1Handler implements IPC.HANDLER_TYPE {
    msg1Handler(String theClientData) { clientData = theClientData; }
    public void handle (IPC.MSG_INSTANCE msgRef, Object callData) {
      System.out.println("msg1Handler: Receiving "+
			 IPC.msgInstanceName(msgRef) +" ("+ callData
			 +") ["+ clientData +"]");
    }
    String clientData;
  }

  private static class queryHandler implements IPC.HANDLER_TYPE {
    queryHandler(String theClientData) { clientData = theClientData; }
    public void handle (IPC.MSG_INSTANCE msgRef, Object callData) {
      System.out.println("queryHandler: Receiving "+
			 IPC.msgInstanceName(msgRef) +" ["+ clientData +"]");
      System.out.println(callData.toString());

      /* Publish this message -- all subscribers get it */
      String str1 = "Hello, world";
      System.out.println("\n  IPC.publishData(\""+ MSG2 +"\", \""+ str1 +"\")");
      IPC.publishData(MSG2, str1);

      T2 t2 = new T2();
      t2.str1 = str1;
      /* Variable length array of one element */
      t2.t1 = new T1[1];
      t2.t1[0] = (T1)callData;
      t2.count = 1;
      t2.status = ReceiveVal;

      /* Respond with this message -- only the query handler gets it */
      System.out.println("\n  IPC.respondData("+ msgRef +", \""+
			 RESPONSE1 +"\", "+ t2 +")");
      IPC.respondData(msgRef, RESPONSE1, t2);
    }
    String clientData;
  }

  private static class stdinHnd implements IPC.FD_HANDLER_TYPE {
    stdinHnd(String theClientData) { clientData = theClientData; }
    public void handle (int fd) {
      try {
	  int in = System.in.read();

	  if (in == 'q' || in == 'Q') {
	    IPC.disconnect();
	    System.exit(-1);
	  } else {
	    System.out.println("stdinHnd ["+ clientData +"]: Received "+ 
			       (char)in);
	  }
	  // Read in any extra bytes
	while (System.in.available() > 0) System.in.read();
      } catch (Exception e) { e.printStackTrace(); }
    }
    String clientData;
  }

  public static void main (String args[]) throws Exception {
    /* Connect to the central server */
    System.out.println("\nIPC.connect(\""+ MODULE2_NAME +"\")");
    IPC.connect(MODULE2_NAME);

    /* Define the messages that this module publishes */
    System.out.println("\nIPC.defineMsg(\""+ MSG2 +"\", \""+
		       MSG2_FORMAT +"\")");
    IPC.defineMsg(MSG2, MSG2_FORMAT);

    System.out.println("\nIPC.defineMsg(\""+ RESPONSE1 +"\", \""+
		       RESPONSE1_FORMAT +"\")");
    IPC.defineMsg(RESPONSE1, RESPONSE1_FORMAT);

    /* Subscribe to the messages that this module listens to. */
    System.out.println("\nIPC.subscribeData(\""+ MSG1 +"\", new msg1Handler(\""+
		       MODULE2_NAME +"\"), int.class)");
    IPC.subscribeData(MSG1, new msg1Handler(MODULE2_NAME), int.class);

    System.out.println("\nIPC.subscribeData(\""+ QUERY1 
		       +"\", new queryHandler(\""+
		       MODULE2_NAME +"\"), T1.class)");
    IPC.subscribeData(QUERY1, new queryHandler(MODULE2_NAME), T1.class);

    /* Subscribe a handler for tty input. Typing "q" will quit the program. */
    System.out.println("\nIPC_subscribeFD(0, new stdinHnd(\""+ 
		       MODULE1_NAME +"\"))");
    IPC.subscribeFD(0, new stdinHnd(MODULE1_NAME));

    System.out.println("\nType 'q' to quit");
    IPC.dispatch();

    IPC.disconnect();
  }
}

