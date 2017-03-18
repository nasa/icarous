/* 
 * ErrorLog
 *
 * Contact: Jeff Maddalon (j.m.maddalon@nasa.gov)
 *
 * Copyright (c) 2011-2017 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 *  
 */

package gov.nasa.larcfm.Util;

public final class ErrorLog implements ErrorReporter {

  private static final String eol = System.getProperty("line.separator");

  private String name;
  private StringBuffer message;
  private boolean has_error;
  private boolean fail_fast;
  private boolean console_out;
  private static int limit = 25;
  private int size;

  // these two flags are used to institute a global clearing of all messages in all ErrorReporters
  // if the local version != the global version, purge this instance before performing the next add operation or copy constructor
  private static int global_purge_flag = 0;
  private int local_purge_flag; 

  private static boolean global_console_out = false;

  /**
   * Construct a new ErrorLog object.  Every message includes the name of this error log.
   * @param name the name of this error log
   */
  public ErrorLog(String name) {
    this.name = name;
    message = new StringBuffer(256);
    has_error = false;
    fail_fast = false;
    console_out = false;
    local_purge_flag = global_purge_flag;
    size = 0;
  }

  /** Construct a new errorlog that is a copy of an existing one without nesting messages. 
   *  @param e the original errorlog
   */
  public ErrorLog(ErrorLog e) {
    if (e.local_purge_flag != global_purge_flag) {
      e.has_error = false;
      e.message.setLength(0);
      e.local_purge_flag = global_purge_flag;
    }
    this.name = new String(e.name);
    this.message = new StringBuffer(e.message);
    this.has_error = e.has_error;
    this.fail_fast = e.fail_fast;
    this.console_out = e.console_out;
    this.local_purge_flag = e.local_purge_flag;
    this.size = e.size;
  }

  /**
   * If fail fast is true, then when an error is added, the program will terminate.
   * @param ff true indicates that the program should terminate when an error is added.
   */
  public void setFailFast(boolean ff) {
    fail_fast = ff;
  }

  /**
   * Set this maximum number of messages that can be held in any given ErrorLog
   * @param sz number of messages to store per log
   */
  public static void setSizeLimit(int sz) {
    limit = sz;
  }


  /**
   * If console output is set to true, then every error message and warning is 
   * output to the console.
   * 
   * @param console true, if output should go to console.
   */
  public void setConsoleOutput(boolean console) {
    console_out = console;
  }

  /**
   * Set a flag that causes all instances to clear their message buffers prior to the next add operation (Note: Java also has an explicit copy constructor).
   * Messages will be preserved if only getMessageNoClear() is called.
   */
  public static void purge() {
    global_purge_flag = (global_purge_flag+1) % 10000;
  }

  /**
   * Indicate that the given error has occurred.
   * @param msg the error message
   */
  public void addError(String msg) {
    if (local_purge_flag != global_purge_flag) {
      message.setLength(0);
      local_purge_flag = global_purge_flag;
    }
    has_error = true;
    message.append("ERROR in ");
    message.append(name);
    message.append(':');
    message.append(' ');
    message.append(msg);
    message.append(eol);
    //System.out.println(message.toString());
    //Thread.dumpStack();
    //System.out.flush();
    //System.err.flush();
    if (fail_fast) {
      System.out.println(message.toString());
      //Thread.dumpStack();			
      System.exit(1);
    }
    if (console_out || global_console_out) {
      System.out.println("ERROR in "+name+": "+msg);
      //			Thread.dumpStack();			
    }
    size++;
    if (size > limit) {
      int idx = message.indexOf(eol);
      message.delete(0, idx);
      message.insert(0,"[...] ");
      size--;
    }
  }

  /**
   * Indicate that the given warning has occurred.
   * @param msg the error message
   */
  public void addWarning(String msg) {
    if (local_purge_flag != global_purge_flag) {
      has_error = false;
      message = new StringBuffer(256);
      local_purge_flag = global_purge_flag;
    }
    message.append("Warning in ");
    message.append(name);
    message.append(": ");
    message.append(msg);
    message.append(eol);
    //System.out.println(message.toString());
    //Thread.dumpStack();
    //System.out.flush();
    //System.err.flush();
    if (console_out || global_console_out) {
      System.out.println("Warning in "+name+": "+msg);
      //			Thread.dumpStack();			
    }
    size++;
    if (size > limit) {
      int idx = message.indexOf(eol);
      message.delete(0, idx);
      message.insert(0,"[...] ");
      size--;
    }
  }

  /**
   * Add all the errors and warnings from the given ErrorReporter into the
   * current ErrorLog.  The given ErrorReporter's status (as indicated
   * by the hasError() and hasMessage() messages) is
   * always clear at the end of this method.
   *
   * @param reporter the ErrorReporter to take messages from
   */
  public void addReporter(ErrorReporter reporter) {
    if (reporter.hasError()) {
      addError(reporter.getMessage());
      return;
    }

    if (reporter.hasMessage()) {
      addWarning(reporter.getMessage());
      return;
    }
  }

  public static void setGlobalConsoleOutFlag(boolean b) {
    global_console_out = b;
  }

  /** 
   * Checks if a value is positive and, in that case, returns true. Otherwise, 
   * adds an error message and returns false.
   * 
   * @param method name of method
   * @param val value
   * @return true, if value is positive
   */
  public boolean isPositive(String method, double val) {
    if (val > 0) return true;
    addError("["+method+"] Value "+val+" is non positive");
    return false;
  }

  /** 
   * Checks if a value is non negative and, in that case, returns true. Otherwise, 
   * adds an error message and returns false.
   * 
   * @param method name of method
   * @param val value
   * @return true, if value is non-negative
   */
  public boolean isNonNegative(String method, double val) {
    if (val >= 0) return true;
    addError("["+method+"] Value "+val+" is negative");
    return false;
  }

  /** 
   * Checks if a value is non positive and, in that case, returns true. Otherwise, 
   * adds an error message and returns false.
   * 
   * @param method name of method
   * @param val value
   * @return true, if value is non-positive
   */
  public boolean isNonPositive(String method, double val) {
    if (val <= 0) return true;
    addError("["+method+"] Value "+val+" is positive");
    return false;
  }

  /** 
   * Checks if a value is less than value in internal units. Otherwise,
   * adds an error message and returns false.
   * 
   * @param method name of method
   * @param val value #1
   * @param thr value #2
   * @return true, if value #1 is less than value #2
   */
  public boolean isLessThan(String method, double val, double thr) {
    if (val < thr) return true;
    addError("["+method+"] Value "+val+" is greater or equal than "+thr);
    return false;
  }

  /** 
   * Checks if a value is between lb and ub. Otherwise, adds an error message 
   * and returns false.
   * 
   * @param method name of method
   * @param val value
   * @param lb  lower bound
   * @param ub  upper bound
   * @return true, if value is between upper and lower bound
   */
  public boolean isBetween(String method, double val, double lb, double ub) {
    if (lb <= val && val <= ub) return true;
    addError("["+method+"] Value "+val+" is not between "+lb+" and "+ub);
    return false;
  }

  // Interface methods

  public boolean hasError() {
    return has_error;
  }

  public boolean hasMessage() {
    return message.length() > 0;
  }

  public String getMessage() {
    has_error = false;
    String rtn = getMessageNoClear();
    message.setLength(0);
    size = 0;
    return rtn;
  }

  public String getMessageNoClear() {
    return message.toString();
  }
}
