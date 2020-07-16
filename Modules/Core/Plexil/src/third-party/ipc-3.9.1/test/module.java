/******************************************************************************
 * PROJECT: New Millennium, DS1
 *          IPC (Interprocess Communication) Package
 *
 * (c) Copyright 2002 Reid Simmons.  All rights reserved.
 *
 * FILE: module.java
 *
 * ABSTRACT: Public class file for module1, module2 and module3 test programs
 *
 * $Revision: 2.2 $
 * $Date: 2009/01/12 15:54:58 $
 * $Author: reids $
 *
 * Copyright (c) 2008, Carnegie Mellon University
 *     This software is distributed under the terms of the 
 *     Simplified BSD License (see ipc/LICENSE.TXT)
 *
 * REVISION HISTORY
 *
 * $Log: module.java,v $
 * Revision 2.2  2009/01/12 15:54:58  reids
 * Added BSD Open Source license info
 *
 * Revision 2.1  2002/01/02 21:13:11  reids
 * Added test files for the Java version of IPC, corresponding to the
 *   C and Lisp test files that already existed.
 *
 ****************************************************************/

public class module {
  /* STATUS_ENUM */
  protected static final int WaitVal    = 0;
  protected static final int SendVal    = 1;
  protected static final int ReceiveVal = 2;
  protected static final int ListenVal  = 3;

  protected static class T1 {
      public int i1;
      public int status; /* STATUS_ENUM */
      public double matrix[/*2*/][/*3*/];
      public double d1;
      public String toString () {
	String str = "{" + i1 +", "+ Integer.toString(status) +", [";
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
	return str +", "+ d1 +"}";
      }
  }

  protected static final String T1_NAME  = "T1";
  // First form of "enum". 3 is the maximum value -- i.e., the value of WaitVal
  protected static final String  T1_FORMAT =
      "{int, {enum : 3}, [double:2,3], double}";

  protected static class T2 {
    public String str1;
    public int count;
    public T1 t1[]; /* Variable length array of type T1_TYPE */
    public int status; /* STATUS_ENUM */

    public String toString () {
      String str = "{\"" + str1 +"\", "+ count +", ";
      str += "<";
      for (int i = 0; i<count; i++) str += t1[i].toString();
      str += ">, ";
      str += (status == WaitVal ? "WaitVal"
	      : status == SendVal ? "SendVal"
	      : status == ReceiveVal ? "ReceiveVal"
	      : status == ListenVal ? "ListenVal" : Integer.toString(status));
      return str +"]";
    }
  }

  protected static final String  T2_NAME = "T2";
  // Alternate form of "enum".
  protected static final String  T2_FORMAT =
      "{string, int, <T1:2>, {enum WaitVal, SendVal, ReceiveVal, ListenVal}}";

  protected static final String  MSG1        = "message1";
  protected static final String  MSG1_FORMAT = "int";

  protected static final String  MSG2        = "message2";
  protected static final String  MSG2_FORMAT = "string";

  protected static final String  QUERY1        = "query1";
  protected static final String  QUERY1_FORMAT = T1_NAME;

  protected static final String  RESPONSE1        = "response1";
  protected static final String  RESPONSE1_FORMAT = T2_NAME;

  protected static final String  MODULE1_NAME = "module1";
  protected static final String  MODULE2_NAME = "module2";
  protected static final String  MODULE3_NAME = "module3";
}
