/* 
 * SeparatedInput
 *
 * Contact: Jeff Maddalon (j.m.maddalon@nasa.gov), Cesar Munoz, George Hagen
 *
 * Copyright (c) 2011-2019 United States Government as represented by
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
	bunits = false;
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
	bunits = false;
	caseSensitive = true;
	header_str.reserve(10); // note: reserve() is appropriate because we exclusively use push_back to populate the vectors
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
	bunits = x.bunits;
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
	preambleImage = x.preambleImage;


	//    std::cout << "SeparatedInput copy constructor failure" << std::endl;
	//    exit(1);
}

// This should never be used, it should exit
SeparatedInput& SeparatedInput::operator=(const SeparatedInput& x) {
	error = x.error;
	reader = x.reader;
	header = x.header;
	bunits = x.bunits;
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
		bunits = true;
	} catch (int e) {
		string outstr = "No exception";
		if (e == 1) outstr = "In parsing fixed width file, number of names does not match number of widths";
		if (e == 2) outstr = "In parsing fixed width file, number of units does not match number of widths";
		error.addError(outstr);
	}
}

void SeparatedInput::setQuoteCharacter(char q) {
		// the easy case
		if (q == 0) {
			quoteCharDefined = false;
			return;
		}
		if (matches(std::string(&q,1), patternStr)) {
			error.addError("setQuoteCharacter: quote character is in list of column delimiters.");
			quoteCharDefined = false; // ignore this setQuoteCharacter()
			return;				
		}
		quoteCharacter = q;
		quoteCharDefined = true;
	}

// q = 0 is the same as setting the quote character to null in java
void SeparatedInput::setQuoteCharacter(char q, const std::vector<char>& delims, const std::string& sub) {
	setQuoteCharacter(q);
}

void SeparatedInput::setCsv() {
	setColumnDelimiters("[ \t]*,[ \t]*");
	setQuoteCharacter('"');
}

char SeparatedInput::getQuoteCharacter() const {
	if ( ! quoteCharDefined) return 0;
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
	return static_cast<int>(header_str.size());
}

int SeparatedInput::findHeading(const std::string& heading) const {
	int rtn = -1;
	for (size_t i = 0; i < header_str.size(); i++) {
		if (heading.compare(header_str[i]) == 0) {
			rtn = static_cast<int>(i);
			break;
		}
	}
	return rtn;
}

string SeparatedInput::getUnit(int i) const {
	if (!bunits || i < 0 || (unsigned int) i >= units_str.size()) {
		return "unspecified";
	}
	return units_str[i];
}

bool SeparatedInput::unitFieldsDefined() const {
	return bunits;
}

void SeparatedInput::setCaseSensitive(bool b) {
	caseSensitive = b;
}

bool SeparatedInput::getCaseSensitive() const {
	return caseSensitive;
}

double SeparatedInput::getUnitFactor(int i) const {
	if (!bunits || i < 0 || (unsigned int) i >= units_str.size()) {
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






bool SeparatedInput::readLine() {
	//char* linestr = new char[maxLineSize];
	string str;
	str.reserve(maxLineSize);
	str.clear();
	try {
		while( ! reader->eof()) {
//		while ( ! reader->eof()) {
//			//reader->getline(linestr, maxLineSize);
//			getline(*reader, str);
//			++linenum;

			str = readFullLine(reader);

			string lineRead = str + "\n";

			// Remove comments from line
			size_t comment_num = str.find('#');
			if (comment_num != string::npos) {   //if (comment_num >= 0) {
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
			} else if ( ! bunits) {
				try {
					bunits = process_units(str);
				}
				catch (SeparatedInputException e) {
					// use default units
					bunits = false;
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


string SeparatedInput::readFullLine(std::istream* reader) {
//		while ( ! reader->eof()) {
//			//reader->getline(linestr, maxLineSize);
//			getline(*reader, str);
//			++linenum;

	string t1;
	getline(*reader, t1);
	++linenum;
	if (reader->eof()) return t1;
	if (quoteCharDefined) {
		do {
			int count = 0;
			for (unsigned long i = 0; i < t1.length(); i++) {
				if (t1[i] == quoteCharacter) count++;
			}
			if (count %2 == 0) break;
			string t2;
			getline(*reader, t2);
			++linenum;
			if (reader->eof()) break;
			t1 = t1 + "\n" + t2;
		} while (true);
	}
	return t1;
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
				fields[i] = substring(str, idx, static_cast<int>(str.length()));
			} else {
				fields[i] = "";
			}
			//fpln("process line i="+Fm2(i)+" field="+fields[i]);
			idx = idx + width_int[i];
		}
	} else {
		std::string str2 = str;
		if (quoteCharDefined) {
			fields = processQuotes(str);
		} else {
			fields = split_regex(str2, patternStr);
		}
	}
	line_str = fields;
}

vector<string> SeparatedInput::processQuotes(const string& str) const {
	vector<string> rtn;
	string temp = "";
		
	if ( ! quoteCharDefined) return rtn;
	bool squote = true;  // true means the next quote character is a starting quote
	string split1;
	split1.push_back(quoteCharacter);
	split1.push_back(quoteCharacter);
	string split2;
	split2.push_back(quoteCharacter);

	vector<string> fields_i = split_string_empty(str, split1);
	for (unsigned long i=0; i<fields_i.size(); i++) {
	    string str_i = fields_i[i];
		//fpln("X"+str_i);
		if ( str_i != "") {
			vector<string> fields_j = split_string_empty(str_i,split2);
			for (unsigned long j=0; j<fields_j.size(); j++) {
				string str_j =fields_j[j];
				//fpln(" Y"+str_j+"Y");
				if ( str_j != "") {
					if ( ! squote) {
						//fpln("  ZZ"+str_j+"ZZ");
						temp = temp + str_j;
					} else {
						vector<string> fields_k = split_regex(str_j,patternStr);
						for (unsigned long k=0; k<fields_k.size(); k++) {
							string str_k = fields_k[k];
							temp = temp + str_k;
							//fpln("  Z"+str_k+"Z");
							if (fields_k.size() - 1 != k) {
								rtn.push_back(temp);
								temp = "";
							}
						}
					}						
				}
				if (fields_j.size()>1 && fields_j.size() - 1 != j) {
					squote = ! squote;
				}  
			}
		}
		if (fields_i.size() - 1 != i) {
			temp = temp + quoteCharacter;
		}
	}
	if ( temp != "") {
		rtn.push_back(temp);
	}
	//fpln("NEXT");
	return rtn;
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
