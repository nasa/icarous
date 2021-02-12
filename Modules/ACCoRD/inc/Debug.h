/*
 * Copyright (c) 2011-2020 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#ifndef DEBUG_H_
#define DEBUG_H_

#include <string>

namespace larcfm {
/**
 * <p>This class contains a small set of tools to help in debugging.  First is a set of tools to instrument the code
 * each type of instrumentation attempts to address a different type of bug.  Any messages go to the Standard output channel.
 * messages come out in the format "<code>&lt;tag&gt; message</code>" so they can easily be found. For errors, the tag
 * is pre-populated with "ERROR!", for warnings the tag is user specified.  For Status messages, the tag is optional.</p>
 * 
 * <p>Unfortunately, this implementation every Debug method added still costs execution time, regardless if
 * the given verbosity level means no message will be produced.  So Debug.pln(lvl, big-complicated-string-operation)
 * means the big string operation will be computed every time, even when the Debug.pln is never triggered.</p>
 * 
 *  <p>Each of these debugging messages approximately means</p>
 * <ul>
 * <li>Error - Usually indicates a software error.  Something where the program is confused.  The requested operation will need to be ignored 
 *     (or perhaps the program must exit). 
 * <li>Warning - Usually indicates a condition that should not be reached, but the software can "fix" the situation.  
 *     The fix may or may not be the intent, hence the warning.
 * <li>Status - Anything else
 * </ul>
 * 
 * The main instrumentation methods are
 * <ul>
 * <li><code>checkError(condition, msg)</code> If the condition is violated, then the message is output.  If the <i>FAIL_FAST</i> flag is true, 
 * the program will exit. 
 * <li><code>error(msg)</code> This method will always display the <i>msg</i> to 
 * the console. This should be reserved for true errors, not curious or questionable situations.    If the 
 * <i>FAIL_FAST</i> flag is true, then this method will force an immediate program exit.
 * <li><code>checkWarning(condition, tag, msg)</code> If the condition is violated, then the message is displayed to the console.
 * <li><code>warning(tag, msg)</code> Writes the message to the console (in the format described above).  
 * <li><code>pln(tag, msg)</code> This provides intermediate program state information.  If the Debug class is in "verbose" mode, 
 * then these messages will be output, if Debug is in "silent" mode, then these messages will be suppressed.  
 * <li><code>pln(msg)</code> This provides intermediate program state information.  Only the message comes out, there is no "tag"
 * <li><code>pln(msg, verbose)</code> Same as above, except it does not rely Debug's notion of a verbosity level.  If the verbose flag is true, then the
 * message is output.
 * <li><code>pln(lvl, msg)</code> This provides information at a user-specified level (>= 2).  Only the message comes out, there is no "tag"
 * <li><code>pln(lvl, tag, msg)</code> Same as above, along with a user-specified tag.
 * </ul>
 * 
 * Usage scenarios
 * <ul>
 * <li>Ensure that conditions that should "never happen" truly never happen.  Instrument the areas of the code that should never
 * be reached with <code>checkError</code> or <code>error</code> methods.  While in development the <i>FAIL_FAST</i> flag should be true.  When the software is
 * ready for distribution, it should be set to true.  If a field report comes back with unusual behavior, search the log for any
 * messages that start with "&lt;ERROR!&gt;".
 * <li>An external user can't get the software to work.  Instruct them to call <code>setVerbose(2)</code>.  (I presume you will have
 * some way in the user interface to set this flag). Then let them
 * examine the log to see if they can determine the issue, and likewise the log can be sent to developers. 
 * </ul>
 * 
 * Future work
 * <ul>
 * <li> Send this information to log files instead of the console.
 * <li> Store the previous message in a string so a GUI can pick it up
 * </ul>
 */
class Debug{
private:
	static const bool FAIL_FAST;
	static int VERBOSE;
	static void output(const std::string& tag, const std::string& msg);
public:
	
	static void setUserDebugFlag(bool b);

	// For compatibility with Java
	/**
	 * Set the verbosity level for debuggging
	 * <ul>
	 * <li> 0 - Errors only
	 * <li> 1 - Errors and Warnings
	 * <li> 2 - Errors, Warnings, and Status
	 * <li> >2 - All the above, plus user-specified levels
	 * </ul>
	 * 
	 * @param level verbosity level
	 */
	static void setVerbose(int level);

	/**
	 * Print out a status message <i>msg</i>, with each line prepended with tag.
	 * The output will only come out if Debug is in "verbose" mode. Note: 
	 * There is a (small) performance penalty for every call, even if the VERBOSE level
	 * indicates no message will come out.
	 * 
	 * @param tag the tag to indicate the location of this debug message.
	 * @param msg the status message
	 */
	static void pln(const std::string& tag, const std::string& msg);
	static void pln2(const std::string& msg, bool verbose);
	/**
	 * Print out a status message <i>msg</i>.
	 * The output will only come out if 2 is below the Debug verbosity level. Note: 
	 * There is a (small) performance penalty for every call, even if the VERBOSE level
	 * indicates no message will come out.
	 * 
	 * @param msg the status message
	 */
	static void pln(const std::string& msg);
	/**
	 * Print out a status message <i>msg</i>, with each line prepended with tag.
	 * The output will only if "verbose" is true.  
	 * 
	 * @param tag the tag to indicate the location of this debug message.
	 * @param msg the status message
	 * @param verbose if true, then display status message
	 */
	static void pln(const std::string& tag, const std::string& msg, bool verbose);
	static void pln(int lvl, const std::string& tag, const std::string& msg);
	static void pln(int lvl, const std::string& msg);

	/**
	 * Output the <i>msg</i> to the console with the prepended <i>tag</i>.  Warnings are always
	 * output and never cause a program termination.
	 * 
	 * @param tag the tag to indicate the location of this debug message.
	 * @param msg message to indicate what has gone wrong.
	 */
	static void warning(const std::string& tag, const std::string& msg);
	/**
	 * Output the <i>msg</i> to the console with the prepended <i>WARNING</i> tag.  Warnings are always
	 * output and never cause a program termination.
	 * 
	 * @param msg message to indicate what has gone wrong.
	 */
	static void warning(const std::string& msg);

	/** 
	 * Output the message <i>msg</i>, indicating that an error has occurred.  
	 * 
	 * @param msg message
	 * @param fail_fast if true, then halt program.  If false, continue. 
	 */
	static void error(const std::string& msg, bool fail_fast);
	/** 
	 * Output the message <i>msg</i>, indicating that an error has occurred.  If in 
	 * fail-fast mode, then the program will exit (perhaps
	 * with a stack trace).
	 * 
	 * @param msg message
	 */
	static void error(const std::string& msg);

	/**
	 * backtrace will only work under gcc
	 */
	static void printStack();

	/**
	 * Indicate than something bad has happened and the program to needs to end now.  On some platforms, print backtrace.
	 * In general, this method should not be used, use {@link error} instead.
	 *  */
	static void halt();
};
}
#endif /* FORMAT_H_ */
