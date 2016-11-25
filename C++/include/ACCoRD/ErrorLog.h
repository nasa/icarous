/* 
 * ErrorLog
 *
 * Contact: Jeff Maddalon (j.m.maddalon@nasa.gov)
 *
 * Copyright (c) 2011-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */


#ifndef ERRORLOG_H_
#define ERRORLOG_H_

#include "ErrorReporter.h"
#include <string>

namespace larcfm {

class ErrorLog : public ErrorReporter {
private:
	std::string name;
	std::string message;
	bool has_error;
	bool fail_fast;
	bool console_out;
	int local_purge_flag;
	static int global_purge_flag;
	static int limit;
	int size;

public:
	/**
	 * Construct a new ErrorLog object.  Every message includes the name of this error log.
	 * @param name the name of this error log
	 */
	ErrorLog(const std::string& name);

	/**
	 * If fail fast is true, then when an error is added, the program will terminate.
	 * @param ff true indicates that the program should terminate when an error is added.
	 */
	void setFailFast(bool ff);

	/**
	 * If console output is set to true, then every error message and warning is
	 * output to the console.
	 */
	void setConsoleOutput(bool console);

	/**
	 * Indicate that the given error has occurred.
	 * @param msg the error message
	 */
	void addError(const std::string& msg);

	/**
	 * Indicate that the given warning has occurred.
	 * @param msg the error message
	 */
	void addWarning(const std::string& msg);

	/**
	 * Add all the errors and warnings from the given ErrorReporter into the
	 * current ErrorLog.  The given ErrorReporter's status (as indicated
	 * by the hasError() and hasMessage() messages) is
	 * always clear at the end of this method.
	 *
	 * @param reporter the ErrorReporter to take messages from
	 */
	void addReporter(ErrorReporter& reporter);

	/**
	 * Set a flag that causes all instances to clear their message buffers prior to the next add operation (Note: Java also has an explicit copy constructor).
	 * Messages will be preserved if only getMessageNoClear() is called.
	 */
	static void purge();

	/**
	 * Set this maximum number of messages that can be held in any given ErrorLog
	 * @param sz number of messages to store per log
	 */
	static void setSizeLimit(int sz);

	/**
	 * Checks if a value is positive and, in that case, returns true. Otherwise,
	 * adds an error message and returns false.
	 */
	bool isPositive(const std::string& method, double val);

	/**
	 * Checks if a value is non negative and, in that case, returns true. Otherwise,
	 * adds an error message and returns false.
	 */
	bool isNonNegative(const std::string& method, double val);

	/**
	 * Checks if a value is non positive and, in that case, returns true. Otherwise,
	 * adds an error message and returns false.
	 */
	bool isNonPositive(const std::string& method, double val);

	/**
	 * Checks if a value is less than value in internal units. Otherwise,
	 * adds an error message and returns false.
	 */
	bool isLessThan(const std::string& method, double val, double thr);

	/**
	 * Checks if a value is between lb and ub. Otherwise, adds an error message
	 * and returns false.
	 */
	bool isBetween(const std::string& method, double val, double lb, double ub);

	bool hasError() const;
	bool hasMessage() const;
	std::string getMessage();
	std::string getMessageNoClear() const;
};

}

#endif /* ERRORLOG_H_ */
