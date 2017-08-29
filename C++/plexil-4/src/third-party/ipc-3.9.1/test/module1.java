/******************************************************************************
 * PROJECT: New Millennium, DS1
 *          IPC (Interprocess Communication) Package
 *
 * (c) Copyright 2002 Reid Simmons.  All rights reserved.
 *
 * FILE: module1.java
 *
 * ABSTRACT: Test program for Java version of IPC.
 *             Publishes: MSG1, QUERY1
 *             Subscribes to: MSG2
 *             Behavior: Sends MSG1 whenever an "m" is typed at the terminal;
 *                       Sends a QUERY1 whenever an "r" is typed;
 *                       Quits the program when a 'q' is typed.
 *                       Should be run in conjunction with module2.
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
 * $Log: module1.java,v $
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

public class module1 extends module {
  private static class msg2Handler implements IPC.HANDLER_TYPE {
    msg2Handler(String theClientData) { clientData = theClientData; }
    public void handle (IPC.MSG_INSTANCE msgRef, Object callData) {
      System.out.println("msg2Handler: Receiving "+
			 IPC.msgInstanceName(msgRef) +" (\""+ callData
			 +"\") ["+ clientData +"]");
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
	} else if (in == 'm' || in == 'M') {
	  int i1 = 42;
	  System.out.println("\n  IPC.publishData(\""+ MSG1 +"\", "+ i1 +")");
	  IPC.publishData(MSG1, i1); 
	} else if (in == 'r' || in == 'R') {
	  T1 t1 = new T1();
	  t1.i1 = 666;
	  t1.status = SendVal;
	  t1.matrix = new double[][] {{0.0, 1.0, 2.0}, {1.0, 2.0, 3.0}};
	  t1.d1 = java.lang.Math.PI;

	  System.out.println("\n  r1 = IPC.queryResponseData(\""+ QUERY1
			     +"\", "+ t1 +", T2.class, IPC.IPC_WAIT_FOREVER)");
	  T2 r1 = (T2)IPC.queryResponseData(QUERY1, t1, T2.class,
					    IPC.IPC_WAIT_FOREVER);
	  System.out.println("\n  Received response: "+ r1.toString());
	} else {
	  System.out.println("stdinHnd ["+ clientData +"]: Received "+(char)in);
	}
	// Read in any extra bytes
	while (System.in.available() > 0) System.in.read();
      } catch (Exception e) { e.printStackTrace(); }
    }
    String clientData;
  }

  private static class handlerChangeHnd implements IPC.CHANGE_HANDLE_TYPE {
      public void handle (String msgName, int num) {
	  System.err.println("HANDLER CHANGE: "+ msgName +": "+ num);
      }
  }

  private static class handlerChangeHnd2 implements IPC.CHANGE_HANDLE_TYPE {
      public void handle (String msgName, int num) {
	  System.err.println("HANDLER CHANGE2: "+ msgName +": "+ num);
      }
  }

  private static class connect1Hnd implements IPC.CONNECT_HANDLE_TYPE {
    public void handle (String moduleName) {
      System.err.println("CONNECT1: Connection from "+ moduleName);
      System.err.println("          Confirming connection ("+
			 IPC.isModuleConnected(moduleName) +")");
    }
  }

  private static class connect2Hnd implements IPC.CONNECT_HANDLE_TYPE {
    public void handle (String moduleName) {
      System.err.println("CONNECT2: Connection from "+ moduleName);
      System.err.println("          Number of handlers: "+
			 IPC.numHandlers(MSG1));
    }
  }

  private static class disconnect1Hnd implements IPC.CONNECT_HANDLE_TYPE {
    static boolean first = true;
    public void handle (String moduleName) {
	System.err.println("DISCONNECT: "+ moduleName);
	if (first) IPC.unsubscribeConnect(connect1Hnd.class);
	else IPC.unsubscribeConnect(connect2Hnd.class);
	if (first) IPC.unsubscribeHandlerChange(MSG1, handlerChangeHnd2.class);
	else IPC.unsubscribeHandlerChange(MSG1, handlerChangeHnd.class);
	first = false;
    }
  }

  public static void main (String args[]) throws Exception {
    // Connect to the central server
    System.out.println("\nIPC.connect(\""+ MODULE1_NAME +"\")");
    IPC.connect(MODULE1_NAME);

    IPC.subscribeConnect(new connect1Hnd());
    IPC.subscribeConnect(new connect2Hnd());
    IPC.subscribeDisconnect(new disconnect1Hnd());

    // Define the named formats that the modules need
    System.out.println("\nIPC.defineFormat(\""+ T1_NAME +"\", \""+
		       T1_FORMAT +"\")");
    IPC.defineFormat(T1_NAME, T1_FORMAT);
    System.out.println("\nIPC.defineFormat(\""+ T2_NAME +"\", \""+
		       T2_FORMAT +"\")");
    IPC.defineFormat(T2_NAME, T2_FORMAT);
  
    // Define the messages that this module publishes
    System.out.println("\nIPC.defineMsg(\""+ MSG1 +"\", \""+
		       MSG1_FORMAT +"\")");
    IPC.defineMsg(MSG1, MSG1_FORMAT);

    IPC.subscribeHandlerChange(MSG1, new handlerChangeHnd());
    IPC.subscribeHandlerChange(MSG1, new handlerChangeHnd2());

    System.out.println("\nIPC.defineMsg(\""+ QUERY1 +"\", \""+
		       QUERY1_FORMAT +"\")");
    IPC.defineMsg(QUERY1, QUERY1_FORMAT);
    IPC.subscribeHandlerChange(QUERY1, new handlerChangeHnd());

    // Subscribe to the messages that this module listens to.
    // NOTE: No need to subscribe to the RESPONSE1 message, since it is a
    //       response to a query, not a regular subscription!
    System.out.println("\nIPC.subscribeData(\""+ MSG2 +"\", new msg2Handler(\""+
		       MODULE1_NAME +"\"), String.class)");
    IPC.subscribeData(MSG2, new msg2Handler(MODULE1_NAME), String.class);

    // Subscribe a handler for tty input.
    //   Typing "q" will quit the program; Typing "m" will send MSG1;
    //   Typing "r" will send QUERY1 ("r" for response)
    System.out.println("\nIPC_subscribeFD(0, new stdinHnd(\""+ 
		       MODULE1_NAME +"\"))");
    IPC.subscribeFD(0, new stdinHnd(MODULE1_NAME));

    System.out.println("\nType 'm' to send "+ MSG1 +"; Type 'r' to send "+
		       QUERY1 +"; Type 'q' to quit");
    IPC.dispatch();

    IPC.disconnect();
  }
}
