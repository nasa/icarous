/* 
 * SeparatedInput
 *
 * Contact: Jeff Maddalon (j.m.maddalon@nasa.gov), Cesar Munoz, George Hagen
 *
 * Copyright (c) 2011-2018 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 *
 */

#include "Units.h"
#include "ErrorLog.h"
#include "ErrorReporter.h"
#include "format.h"
#include "SeparatedInput.h"
#include "Constants.h"
#include "string_util.h"
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <stdexcept>
#include <algorithm>

namespace larcfm {

using std::string;
using std::vector;
using std::map;
using std::cout;
using std::endl;

SeparatedInput::SeparatedInputException::SeparatedInputException(
		const std::string& s) :
						std::logic_error(s) {
}

SeparatedInput::SeparatedInput() :
						error("SeparatedInput") {
	header = false;
	units = false;
	caseSensitive = true;
	parameters = ParameterData();
	patternStr = Constants::wsPatternBase;
	reader = 0;
	linenum = 0;
	fixed_width = false;
	quoteCharDefined = false;
	quoteCharacter = '"'; // not needed, just to eliminate warnings
	preambleImage = "";
}

// fs must already be opened!!!
SeparatedInput::SeparatedInput(std::istream* ins) :
						error("SeparatedInput") {
	reader = ins;
	header = false;
	units = false;
	caseSensitive = true;
	header_str.reserve(10);
	units_str.reserve(10);
	units_factor.reserve(10);
	line_str.reserve(10);
	linenum = 0;
	patternStr = Constants::wsPatternBase;
	fixed_width = false;
	parameters = ParameterData();
	quoteCharDefined = false;
	quoteCharacter = '"'; // not needed, just to eliminate warnings
	preambleImage = "";

}

// This should never be used, it should exit
SeparatedInput::SeparatedInput(const SeparatedInput& x) :
						error("SeparatedInputError") {
	error = x.error;
	reader = x.reader;
	header = x.header;
	units = x.units;
	caseSensitive = x.caseSensitive;
	header_str = x.header_str;
	units_str = x.units_str;
	units_factor = x.units_factor;
	line_str = x.line_str;
	linenum = x.linenum;
	patternStr = x.patternStr;
	parameters = x.parameters;
	fixed_width = x.fixed_width;
	quoteCharDefined = x.quoteCharDefined;
	quoteCharacter = x.quoteCharacter;
	quoteSubstitutions = x.quoteSubstitutions;
	preambleImage = x.preambleImage;


	//    std::cout << "SeparatedInput copy constructor failure" << std::endl;
	//    exit(1);
}

// This should never be used, it should exit
SeparatedInput& SeparatedInput::operator=(const SeparatedInput& x) {
	error = x.error;
	reader = x.reader;
	header = x.header;
	units = x.units;
	caseSensitive = x.caseSensitive;
	header_str = x.header_str;
	units_str = x.units_str;
	units_factor = x.units_factor;
	line_str = x.line_str;
	linenum = x.linenum;
	patternStr = x.patternStr;
	parameters = x.parameters;
	fixed_width = x.fixed_width;
	quoteCharDefined = x.quoteCharDefined;
	quoteCharacter = x.quoteCharacter;
	quoteSubstitutions = x.quoteSubstitutions;
	preambleImage = x.preambleImage;

	//std::cout << "SeparatedInput assignment operator failure" << std::endl;
	//exit(1);
	return *this;  // should never get here
}


void SeparatedInput::setColumnDelimiters(const std::string& delim) {
	patternStr = delim;
}

std::string SeparatedInput::getColumnDelimiters() const {
	return patternStr;
}


void SeparatedInput::setFixedColumn(const string& widths, const string& nameList, const string& unitList) {
	try {
		vector<string> fields = split(widths, ",");

		width_int = vector<int>(fields.size());
		for (unsigned int i = 0; i < fields.size(); i++) {
			width_int[i] = parseInt(fields[i]);
		}

		fields = split(nameList,",");
		if (width_int.size() != fields.size()) {
			throw 1;
		}
		header_str = vector<string>(fields.size());
		for (unsigned int i = 0; i < fields.size(); i++) {
			header_str[i] = fields[i];
		}

		fields = split(unitList,",");
		if (width_int.size() != fields.size()) {
			throw 2;
		}
		units_str = vector<string>(fields.size());
		units_factor = vector<double>(fields.size());
		for (unsigned int i = 0; i < fields.size(); i++) {
			if (Units::isUnit(fields[i])) {
				units_str[i] = fields[i];
				units_factor[i] = Units::getFactor(fields[i]);
			} else if (equals(fields[i],"-")) {
				units_str[i] = "unitless";
				units_factor[i] = Units::unitless;
			} else {
				units_str[i] = "unspecified";
				units_factor[i] = Units::unspecified;
			}
		}

		fixed_width = true;
		header = true;
		units = true;
	} catch (int e) {
		string outstr = "No exception";
		if (e == 1) outstr = "In parsing fixed width file, number of names does not match number of widths";
		if (e == 2) outstr = "In parsing fixed width file, number of units does not match number of widths";
		error.addError(outstr);
	}
}

// q = 0 is the same as setting the quote character to null in java
void SeparatedInput::setQuoteCharacter(char q, const std::vector<char>& delims, const std::string& sub) {
	if (q == 0) {
		quoteCharDefined = false;
		return;
	}
	// there are a bunch of ways this can go wrong.
	if (delims.size() == 0 || sub.size() == 0) {
		error.addError("setQuoteCharacter: substitution lists are empty");
		return;
	}
	if (delims.size() > 99) {
		error.addError("setQuoteCharacter: exceeded 100 delimiter max");
		return;
	}
	for (int i = 0; i < (int) delims.size(); i++) {
		char d = delims.at(i);
		if (d == q) {
			error.addError("setQuoteCharacter: substitution delim list contains the quote character!");
			return;
		}
		if (!matches(std::string(&d,1), patternStr)) {
			error.addError("setQuoteCharacter: substitution delim list does not match column delimiters!");
			return;
		}
	}
	if (sub.size() == 0) {
		error.addError("setQuoteCharacter: substitution subs list null or empty string!");
		return;
	}
	if (sub.find(std::string(&q,1)) != std::string::npos) {
		error.addError("setQuoteCharacter: substitution subs list contains the quote character!");
		return;
	}
	for (int j = 0; j < (int) delims.size(); j++) {
		char d = delims.at(j);
		if (sub.find(std::string(&d,1)) != std::string::npos) {
			error.addError("setQuoteCharacter: substitution subs list contains recursion!");
			return;
		}
	}
	if (matches(sub, patternStr)) {
		error.addError("setQuoteCharacter: substitution subs list matches column delimiters!");
		return;
	}
	quoteCharacter = q;
	quoteSubstitutions = std::map<std::string,std::string>();
	for (int i = 0; i < (int) delims.size(); i++) {
		char d = delims.at(i);
		std::string cs = std::string(&d,1);
		quoteSubstitutions[cs] = sub+FmLead(i,2);
	}
	quoteCharDefined = true;
}

char SeparatedInput::getQuoteCharacter() const {
	if (!quoteCharDefined) return 0;
	return quoteCharacter;
}

ParameterData& SeparatedInput::getParametersRef() {
	return parameters;
}

ParameterData SeparatedInput::getParameters() const {
	return parameters;
}

string SeparatedInput::getHeading(int i) const {
	if (i < 0 || (unsigned int) i >= header_str.size()) { // was line_str.size()
		error.addWarning(
				"getHeading index " + Fm0(i) + ", line " + Fm0(linenum)
				+ " out of bounds");
		return "";
	}
	return header_str[i];
}

int SeparatedInput::size() const {
	return header_str.size();
}

int SeparatedInput::findHeading(const std::string& heading) const {
	int rtn = -1;
	for (unsigned int i = 0; i < header_str.size(); i++) {
		if (heading.compare(header_str[i]) == 0) {
			rtn = i;
			break;
		}
	}
	return rtn;
}

string SeparatedInput::getUnit(int i) const {
	if (!units || i < 0 || (unsigned int) i >= units_str.size()) {
		return "unspecified";
	}
	return units_str[i];
}

bool SeparatedInput::unitFieldsDefined() const {
	return units;
}

void SeparatedInput::setCaseSensitive(bool b) {
	caseSensitive = b;
}

bool SeparatedInput::getCaseSensitive() const {
	return caseSensitive;
}

double SeparatedInput::getUnitFactor(int i) const {
	if (!units || i < 0 || (unsigned int) i >= units_str.size()) {
		return Units::getFactor("unspecified");
	}
	return units_factor[i];
}


bool SeparatedInput::columnHasValue(int i) const {
	return (i >= 0 && i < (int) line_str.size() && !equals(line_str[i], "")
			&& !equals(line_str[i], "-"));
}

string SeparatedInput::getColumnString(int i) const {
	if (i < 0 || (unsigned int) i >= line_str.size()) {
		error.addWarning(
				"getColumnString index " + Fm0(i) + ", line " + Fm0(linenum)
				+ " out of bounds");
		return "";
	}
	return line_str[i];
}

double SeparatedInput::getColumn(int i, double defaultValue, bool verbose) const {
	if (i < 0 || (unsigned int) i >= line_str.size()) {
		if (verbose) error.addWarning("getColumn index " + Fm0(i) + ", line " + Fm0(linenum) + " out of bounds");
		return defaultValue;
	}
	double rtn = defaultValue;
	try {
		rtn = Units::from(getUnitFactor(i), Util::parse_double(line_str[i]));
	}
	catch (std::runtime_error e) {
		if (verbose) error.addWarning("could not parse double (" + Fm0(i) + "), line " + Fm0(linenum) + ": " + line_str[i]);
		rtn = defaultValue;  // arbitrary value
	}
	return rtn;
}

double SeparatedInput::getColumn(int i) const {
	return getColumn(i, 0.0, true);
}


double SeparatedInput::getColumn(int i, const std::string& default_unit) const {
	if (getUnit(i) == "unspecified") {
		return Units::from(default_unit, getColumn(i, 0.0, true));
	}

	return getColumn(i, 0.0, true);
}


// given a string, replace separation delimiters that occur within quoted strings
std::string SeparatedInput::tokenizeQuotes(const std::string& str) const {
	std::string tmp = str;
	std::string qchar = std::string(&quoteCharacter,1);
	int index = tmp.find(qchar,0);
	// only do replacements inside of quotes
	while (index != (int) std::string::npos) {
		int index2 = tmp.find(qchar,index+1);
		if (index2 != (int) std::string::npos) {
			std::string s1 = tmp.substr(0, index);
			std::string s2 = tmp.substr(index+1, index2-(index+1));
			std::string s3 = "";
			if (index2+1 < (int) tmp.length()) {
				s3 = tmp.substr(index2+1, tmp.length()-(index2+1));
			}
			std::map<std::string,std::string>::const_iterator pos;
			for (pos = quoteSubstitutions.begin(); pos != quoteSubstitutions.end(); ++pos) {
				s2 = replace(s2, pos->first, pos->second);
			}
			tmp = s1+s2+s3;
			index2 = (s1+s2).size()-1;
		}
		index = tmp.find(qchar,index2+1);
	}
	return tmp;
}

// given a string that has gone through the above substitution, return the column delimiter characters
std::string SeparatedInput::unTokenizeQuotes(const std::string& str) const {
	std::string tmp = str;
	std::map<std::string,std::string>::const_iterator pos;
	for (pos = quoteSubstitutions.begin(); pos != quoteSubstitutions.end(); ++pos) {
		tmp = replace(tmp, pos->second, pos->first);
	}
	return tmp;
}



bool SeparatedInput::readLine() {
	//char* linestr = new char[maxLineSize];
	string str;
	str.reserve(maxLineSize);
	str.clear();
	try {
		while ( ! reader->eof()) {
			//reader->getline(linestr, maxLineSize);
			getline(*reader, str);
			++linenum;

			string lineRead = str + "\n";

			// Remove comments from line
			int comment_num = str.find('#');
			if (comment_num >= 0) {
				str = str.substr(0,comment_num);
			}
			trim(str);
			// Skip empty lines
			if (str.size() == 0) {
				str.clear();
				if (!header) preambleImage += lineRead; // store image
				continue;
			}


			if (!header) {
				header = process_preamble(str);
				if (!header) {
					preambleImage += lineRead;
				}
			} else if (!units) {
				try {
					units = process_units(str);
				}
				catch (SeparatedInputException e) {
					// use default units
					units = false;
					process_line(str);
					break;
				}
			} else {
				process_line(str);
				break;
			}
			str.clear();
		}  //while
	}
	catch (std::runtime_error e) {
		error.addError(
				"*** An IO error occurred at line " + Fm0(linenum)
				+ "The error was:" + e.what());
		// ERROR CLEANUP
	}
	if (str.size() == 0) {
		return true;
	} else {
		return false;
	}
}

int SeparatedInput::lineNumber() const {
	return linenum;
}

bool SeparatedInput::process_preamble(string str) {
	vector<string> fields = split(str, "=");   // C++ version of split doesn't need the "-2" parameter that java needs.

	// parameter keys are lower case only
	if (fields.size() >= 2	&& fields[0].length() > 0) {
		string id = fields[0];
		trim(id, " \t");
		if ( ! caseSensitive)
			id = toLowerCase(id);

		// string value
		string strv = fields[1];
		for (unsigned int i = 2; i < fields.size(); i++) {
			strv += "=" + fields[i];
		} // values become a space-delineated list string

		parameters.set(id, strv);

		return false;
	} else if (fields.size() == 1 && contains(str,"=")) {
		string id = string(fields[0]);
		trim(id);
		parameters.set(id,"");
		return false;
	} else {
		//fields = split(str, patternStr);
		fields = split_regex(str, patternStr);
		if ( ! caseSensitive) {
			for (unsigned int i = 0; i < fields.size(); i++) {
				fields[i] = toLowerCase(fields[i]);
				trim(fields[i]);
			}
		}
		header_str = fields;
		return true;
	}
}

bool SeparatedInput::process_units(const string& str) {
	//vector<string> fields = split(str, patternStr);
	vector<string> fields = split_regex(str, patternStr);

	// if units are optional, we need to determine if any were read in...
	// a unit line is considered true if AT LEASE HALF of the fields read in are interpreted as valid units
	unsigned int notFound = 0;
	unsigned int dash = 0;

	for (unsigned int i = 0; i < fields.size(); i++) {
		std::string fstr = fields[i];
		trim(fstr);
		if (equals(fstr,"-")) {
			fstr = "unitless";
			dash++;
		}
		std::string u = Units::clean(fstr);
		double factor = Units::getFactor(u);
		if (equals(u,"unspecified")) {
			notFound++;
			units_str.push_back("unspecified");
			units_factor.push_back(Units::getFactor("unspecified"));
		} else {
			units_str.push_back(u);
			units_factor.push_back(factor);
		}
	}
	if (notFound > fields.size() / 2 || notFound+dash == fields.size()) {
		throw SeparatedInputException("default units");
	}
	return true;
}

void SeparatedInput::process_line(const string& str) {
	//vector<string> fields = split(str, patternStr);
	vector<string> fields;
	if (fixed_width) {
		unsigned int idx = 0;
		fields = vector<string>(width_int.size());
		for (unsigned int i = 0; i < width_int.size(); i++) {
			unsigned int end = idx+width_int[i];
			if (idx < str.length() && end <= str.length()) {
				fields[i] = substring(str, idx, idx+width_int[i]);
			} else if (idx < str.length() && end > str.length()) {
				fields[i] = substring(str, idx, str.length());
			} else {
				fields[i] = "";
			}
			//fpln("process line i="+Fm2(i)+" field="+fields[i]);
			idx = idx + width_int[i];
		}
	} else {
		std::string str2 = str;
		if (quoteCharDefined) {
			str2 = tokenizeQuotes(str);
		}
		fields = split_regex(str2, patternStr);
		for (int i = 0; i < (int) fields.size(); i++) {
			trim(fields[i]);
			if (quoteCharDefined) {
				fields[i] = unTokenizeQuotes(fields[i]);
			}
		}
	}
	line_str = fields;
}

string SeparatedInput::getLine() const {
	string s = "";
	if (line_str.size() > 0) {
		s = line_str[0];
	}
	for (int i = 1; i < (int) line_str.size(); i++) {
		s += ", "+line_str[i];
	}
	return s;
}

std::string SeparatedInput::getPreambleImage() const {
	return preambleImage;
}

}
