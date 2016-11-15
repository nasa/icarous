/*
 * Copyright (c) 2013-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
package gov.nasa.larcfm.Util;

public class Debug {
	private static boolean userDebugFlag = false;

	public static void setUserDebugFlag(boolean b) {
		userDebugFlag = b;
	}

	/**
	 * Print out a debugging message msg, with each line prepended with tag
	 * 
	 * @param tag the tag to indicate the location of this debug message.
	 * @param msg the debug message
	 */
	public static void pln(String tag, String msg) {
		if (userDebugFlag) {
			String[] lines = msg.split("\\n");
			for (int i = 0; i < lines.length; i++) {
				System.out.println("<!! "+tag+" !!> "+lines[i]);
			}
		}
	}

	/**
	 * Print out a debugging message.  Be careful with this method, it may be hard to 
	 * find these messages in the future, use pln(tag,msg) instead.
	 * 
	 * @param msg the debug message
	 */
	public static void pln(String msg) {
		pln("empty",msg);
	}

	public static void halt(String str) {
		System.out.println(str);
		if (Constants.allowDebugHalt) {
			Thread.dumpStack();
			System.exit(1);
		}
	}

	public static void halt() {
		halt("");	
	}
	

}

