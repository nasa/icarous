/* 
 * ErrorLog
 *
 * Contact: Jeff Maddalon (j.m.maddalon@nasa.gov)
 *
 * Copyright (c) 2011-2019 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#include "ErrorLog.h"
#include <iostream>
#include "format.h"
#include <cstdlib>

namespace larcfm {

using namespace std;

int ErrorLog::global_purge_flag = 0;
int ErrorLog::limit = 25;

ErrorLog::ErrorLog(const string& logname) :
	name(logname),
	message("") {
	//this->name = string(name);
	//message = "";
	has_error = false;
	fail_fast = false;
	console_out = false;
	local_purge_flag = global_purge_flag;
	size = 0;
}

void ErrorLog::setFailFast(bool ff) {
	fail_fast = ff;
}

void ErrorLog::setConsoleOutput(bool console) {
	console_out = console;
}

void ErrorLog::addError(const string& msg) {
	if (local_purge_flag != global_purge_flag) {
		message = "";
		local_purge_flag = global_purge_flag;
	}
	//fpln(" $$$$ ErrorLog: addError: "+msg);
	has_error = true;
	message += "ERROR in ";
	message += name;
	message += ": ";
	message += msg;
	message += "\n";
	if (fail_fast) {
		cout << message;
		exit(1);
	}
	if (console_out) {
		cout << "ERROR in " << name << ": " << msg << endl;
	}
	size++;
	if (size > limit) {
		size_t idx = message.find("\n");
		message.erase(0, idx);
		message.insert(0,"[...] ");
		size--;
	}

}

void ErrorLog::addWarning(const string& msg) {
	if (local_purge_flag != global_purge_flag) {
		message = "";
		has_error = false;
		local_purge_flag = global_purge_flag;
	}
	message += "Warning in ";
	message += name;
	message += ": ";
	message += msg;
	message += "\n";
	if (console_out) {
		cout << "Warning in " << name << ": " << msg << endl;
	}
	size++;
	if (size > limit) {
		size_t idx = message.find("\n");
		message.erase(0, idx);
		message.insert(0,"[...] ");
		size--;
	}
}

void ErrorLog::addReporter(ErrorReporter& reporter) {
	if (reporter.hasError()) {
		addError(reporter.getMessage());
		return;
	}

	if (reporter.hasMessage()) {
		addWarning(reporter.getMessage());
		return;
	}
}

void ErrorLog::purge() {
	global_purge_flag = (global_purge_flag + 1) % 10000;
}

void ErrorLog::setSizeLimit(int sz) {
	limit = sz;
}

/**
 * Checks if a value is positive and, in that case, returns true. Otherwise,
 * adds an error message and returns false.
 */
bool ErrorLog::isPositive(const std::string& method, double val) {
	if (val > 0) return true;
	addError("["+method+"] Value "+Fm4(val)+" is non positive");
	return false;
}

/**
 * Checks if a value is non negative and, in that case, returns true. Otherwise,
 * adds an error message and returns false.
 */
bool ErrorLog::isNonNegative(const std::string& method, double val) {
	if (val >= 0) return true;
	addError("["+method+"] Value "+Fm4(val)+" is negative");
	return false;
}

/**
 * Checks if a value is non positive and, in that case, returns true. Otherwise,
 * adds an error message and returns false.
 */
bool ErrorLog::isNonPositive(const std::string& method, double val) {
	if (val <= 0) return true;
	addError("["+method+"] Value "+Fm4(val)+" is positive");
	return false;
}

/**
 * Checks if a value is less than value in internal units. Otherwise,
 * adds an error message and returns false.
 */
bool ErrorLog::isLessThan(const std::string& method, double val, double thr) {
	if (val < thr) return true;
	addError("["+method+"] Value "+Fm4(val)+" is greater or equal than "+Fm4(thr));
	return false;
}

/**
 * Checks if a value is between lb and ub. Otherwise, adds an error message
 * and returns false.
 */
bool ErrorLog::isBetween(const std::string& method, double val, double lb, double ub) {
	if (lb <= val && val <= ub) return true;
	addError("["+method+"] Value "+Fm4(val)+" is not between "+Fm4(lb)+" and "+Fm4(ub));
	return false;
}

// Interface methods

bool ErrorLog::hasError() const {
	return has_error;
}

bool ErrorLog::hasMessage() const {
	return message.length() > 0;
}

string ErrorLog::getMessage() {
	has_error = false;
	string rtn = getMessageNoClear();
	message = "";
	size = 0;
	return rtn;
}

string ErrorLog::getMessageNoClear() const {
	return message;
}

}
