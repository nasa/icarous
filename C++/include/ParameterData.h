/*
 * Copyright (c) 2014-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#ifndef PARAMETERDATA_H_
#define PARAMETERDATA_H_

#include "Units.h"
#include "ErrorLog.h"
#include "ErrorReporter.h"
#include "format.h"
#include "Quad.h"
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

struct stringCaseInsensitive {
  bool operator() (const std::string& lhs, const std::string& rhs) const
  {return toLowerCase(lhs)<toLowerCase(rhs);}
};

/**
 * This class stores a database of parameters. In addition, it performs various
 * operations parameter strings, including parsing some more complicated
 * parameter strings into various data structures.
 */
class ParameterData {

public:
	static const std::string parenPattern;
    static const std::string defaultEntrySeparator;


	ParameterData();
	bool isCaseSensitive();
	void setCaseSensitive(bool v);
	bool isPreserveUnits();
	void setPreserveUnits(bool v);
	bool isUnitCompatibility();
	void setUnitCompatibility(bool v);

	int size();
	/**
	 * Returns an array of parameter strings encountered.
	 */
	std::vector<std::string> getList() const;
	/**
	 * Returns an array of parameter strings encountered.
	 */
	std::vector<std::string> getListFull() const;
	/**
	 * Removes all stored parameters.
	 */
	void clear();
	/**
	 * Returns true if the parameter key was defined.
	 */
	bool contains(const std::string& key) const;

	std::vector<std::string> matchList(const std::string& key) const;

	ParameterData copyWithPrefix(const std::string& prefix) const;

	ParameterData extractPrefix(const std::string& prefix) const;

	/**
	 * Returns the string value of the given parameter key. This may be a
	 * space-delimited list. If the key is not present, return the empty string.
	 * Parameter keys may be case-sensitive.
	 */
	std::string getString(const std::string& key) const;

	/**
	 * Returns the double-precision value of the given parameter key in internal
	 * units. If the key is not present or if the value is not a numeral, then
	 * return 0. Parameter keys may be case-sensitive.
	 * 
	 * 
	 * 
	 * 
	 * 
	 * 
	 * Returns the double-precision value of the given parameter key. If units
	 * were specified in the file, this value has been converted into internal
	 * units. If no units were specified, then the value in the file is
	 * returned. If the key is not present or if the value is not a numeral,
	 * then return 0. Parameter keys may be case-sensitive.
	 */
	double getValue(const std::string& key) const;
	/**
	 * Returns the double-precision value of the given parameter key in internal
	 * units. Only in the case when units were not specified in the file, will
	 * the defaultUnit parameter be used. If the key is not present or if the
	 * value is not a numeral, then return 0. Parameter keys may be
	 * case-sensitive.
	 * 
	 * 
	 * 
	 * Returns the double-precision value of the given parameter key in internal
	 * units. If no units were specified in the file, then the defaultUnit
	 * parameter is used. If units were specified in the file, then the
	 * defaultUnit parameter is ignored. If the key is not present or if the
	 * value is not a numeral, then return 0. Parameter keys may be
	 * case-sensitive.
	 */
	double getValue(const std::string& key, const std::string& defaultUnit) const;
	/**
	 * Returns the string representation of the specified unit of the given
	 * parameter key. If the key is not present or no unit was specified, return
	 * "unspecified". Parameter keys may be case-sensitive.
	 */
	std::string getUnit(const std::string& key) const;
	/**
	 * Returns the Boolean value of the given parameter key. If the key is not
	 * present, or not representation of "true", return the empty string.
	 * Parameter keys may be case-sensitive.
	 */
	bool getBool(const std::string& key) const;
    /**
     * Returns the integer value of the given parameter key in internal units.
     * If no units were specified in the file, then the defaultUnit parameter is used.
     * If the key is not present or if the 
     * value is not a numeral, then return 0.  This value is an integer version of the 
     * double value (see the related getParameterValue() method).  If the double value is 
     * larger than an integer, behavior is undefined.
     * Parameter keys may be case-sensitive.
     */
    int getInt(const std::string& key) const;
    /**
     * Returns the long value of the given parameter key in internal units.
     * If no units were specified in the file, then the defaultUnit parameter is used.
     * If the key is not present or if the 
     * value is not a numeral, then return 0.  This value is an integer version of the 
     * double value (see the related getParameterValue() method).  If the double value is 
     * larger than an long, behavior is undefined.  
     * Parameter keys may be case-sensitive.
     */
    long getLong(const std::string& key) const;
	/**
	 * Parses the given string as a parameter assignment. If successful, returns
	 * a true value and adds the parameter. Otherwise returns a false value and
	 * makes no change to the parameter database.<p>
	 * 
	 * Examples of valid strings include:
	 * <ul>
	 * <li> a = true
	 * <li> b = hello everyone!
	 * <li> c = 10 [NM]
	 * </ul>
	 */
	bool set(const std::string& s);
	/**
	 * Associates a parameter key with a value (both represented as strings).
	 * The value field may include a units descriptor in addition to the actual
	 * value, usually in a format similar to "10 [NM]", representing 10 nautical
	 * miles.
	 */
	bool set(const std::string& key, const std::string& value);
	bool set(const std::string& key, char* value);
	bool set(const std::string& key, const char* value);
    /** Associates a bool value with a parameter key. */
	bool setTrue(const std::string& key);
	bool setFalse(const std::string& key);
	bool setBool(const std::string& key, bool val);

	// WARNING: DO NOT add this method to C++, bool's in C++ are integers and will
	// capture some calls to .set with integer (and some pointer) parameters. For
	// the case with integer parameters, the desired behavior is to go to the set(string,double,string) version.
    //void set(const std::string& key, bool value);

	/** Associates a value (in the given units) with a parameter key. */
	bool set(const std::string& key, double value, const std::string& unit);
	/** Associates a value (in internal units) with a parameter key. The default units of
	 * this value are provided by the units string.
	 */
	bool setInternal(const std::string& key, double value, const std::string& units);
	bool setInternal(const std::string& key, double value, const std::string& units, int prec);

	/** Associates a value (in the given units) with a parameter key.
	 * If the parameter was already defined with a specified unit than
	 * the original unit is retained as the default unit for this parameter.  If not, then the
	 * unit provided through this method becomes the default unit for
	 * this parameter.
	 *
	 * @param key
	 * @param value
	 * @param units
	 */
	void setPreserveUnit(const std::string& key, double value, const std::string& units);

	/** Associates a value (in internal units) with a parameter key.
	 * If the parameter was already defined with a specified unit than
	 * the original unit is retained as the default unit for this parameter.  If not, then the
	 * unit provided through this method becomes the default unit for
	 * this parameter.
	 *
	 * @param key
	 * @param value
	 * @param units
	 */
	void setInternalPreserveUnit(const std::string& key, double value, const std::string& units);



//	bool setValueOnly(const std::string& key, double value);
//	bool setDefaultUnit(const std::string& key, const std::string& unit);

	/**
	 * Checks the parameters against the supplied list, and returns a list of
	 * unrecognized parameters that have been read, possible empty
	 */
	std::vector<std::string> unrecognizedParameters(std::vector<std::string> c) const;
	/**
	 * Checks the parameters against the supplied list, and returns a list
	 * of unrecognized parameters.
	 */
	std::vector<std::string> validateParameters(std::vector<std::string> c);
	void copy(ParameterData p, bool overwrite);

	/**
	 * Remove this key from the database, if it exsts.
	 */
	void remove(const std::string& key);

	/**
	 * Remove all keys in the list from this database, if they exist.
	 */
	void removeAll(const std::vector<std::string>& key);


	std::string toParameterList(const std::string& separator) const;
	bool parseParameterList(const std::string& separator, std::string line);

	std::string toString() const;

	bool equals(const ParameterData& pd) const;


	std::vector<int> getListInteger(const std::string& key) const;
	std::vector<double> getListDouble(const std::string& key) const;
	std::vector<std::string> getListString(const std::string& key) const;
	std::vector<bool> getListBool(const std::string& key) const;

	bool set(const std::string& key, const std::vector<int>& list);
	bool set(const std::string& key, const std::vector<double>& list);
	bool set(const std::string& key, const std::vector<std::string>& list);
	bool setListBool(const std::string& key, const std::vector<bool>& list);

private:
	bool parse_parameter_string(const std::string& str);

	bool putParam(const std::string& key, const std::pair<bool, Quad<std::string, double, std::string, bool> >& entry);

	std::pair<bool, Quad<std::string, double, std::string, bool> > parse_parameter_value(const std::string& value);

	static std::vector<std::string> stringList(const std::string& instring);
	static std::vector<int> intList(const std::string& instring);
	static std::vector<double> doubleList(const std::string& instring);
	static std::vector<bool> boolList(const std::string& instring);


	bool caseSensitive;
	bool preserveUnits;
	bool unitCompatibility;
	std::string patternStr;
    typedef std::map<std::string, Quad<std::string,double,std::string,bool>, stringCaseInsensitive > paramtype;
    paramtype parameters;


};

}

#endif
