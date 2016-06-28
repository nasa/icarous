/*
 * Copyright (c) 2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
package gov.nasa.larc.ICAROUS;

public class Messenger {

  // runtime params are false unless stated otherwise at runtime
  public static boolean debug = false;
  public static boolean verbose = false;

  /**
   * Print error message to console
   * 
   * @param msg
   */
  public static void err(String msg)
  {
    System.err.println(Thread.currentThread().getName()
        +" (err) --> "
        + msg
        );
  }

  /**
   * Print message (only if debug or verbose modes are active)
   * 
   * @param msg
   */
  public static void msg(String msg)
  {
    if (debug || verbose)
      System.out.println(Thread.currentThread().getName()
          +" (msg) --> "
          + msg
          );
  }

  /**
   * Print debug message (only if debug mode is active)
   * 
   * @param msg
   */
  public static void dbg(String msg)
  {
    if (debug)
      System.out.println(Thread.currentThread().getName()
          +" (dbg) --> "
          + msg
          );
  }

}
