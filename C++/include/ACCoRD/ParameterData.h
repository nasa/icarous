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
 * parameter strings into various data structures.<p>
 * 
 * All key accesses are case insensitive, however the actual key values are stored with case information intact.  Methods returning the 
 * key values (getList, getListFull) will reflect the capitalization scheme used for the initial assignment to a key.
 * 
 */
class ParameterData {

public:
	static const std::string parenPattern;
    static const std::string defaultEntrySeparator;


	ParameterData();
	bool isCaseSensitive();
	void setCaseSensitive(bool v);
	/** 
	 * Will set methods update the units in the database, or will the units
	 * in the database be preserved?
	 * @return true, if the units in the database will be preserved when a set method is called.
	 */
	bool isPreserveUnits();
	/**
	 * If true, then all subsequent calls to "set.." methods will not update 
	 * the units in the database to the units supplied through a "set.." method.
	 * The only exception is if the units in the database are "unspecified" then
	 * the units in the database will be updated with the value of units supplied through 
	 * a "set.." method.
	 * 
	 * @param v true when the units in the database should be preserved
	 */
	void setPreserveUnits(bool v);
	/** 
	 * Will set methods disallow updating a unit if the new unit is incompatible with the
	 * old unit.  Meters and feet are compatible, whereas meters and kilograms are not.
	 * Most of the time, one wants the enforcement that compatible units are required,
	 * but there may be some situations where this is undesired.
	 * @return true, if set methods must use compatible units with the units in the database
	 */
	bool isUnitCompatibility();
	/**
	 * Will set methods disallow updating a unit if the new unit is incompatible with the
	 * old unit.  Meters and feet are compatible, whereas meters and kilograms are not.
	 * Most of the time, one wants the enforcement that compatible units are required,
	 * but there may be some situations where this is undesired.
	 * 
	 * @param v true when the units in a set method must be compatible with the database.
	 */
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

	/**
	 * Return a copy of this ParameterData object, with all key values being prepended with the string prefix.
	 * This is intended to create a unique ParameterData object representing the parameters of a particular 
	 * instance (of many) of an object, that can then be collected along with others into a larger ParameterData object representing the containing object.
	 * @param prefix
	 * @return
	 */
	ParameterData copyWithPrefix(const std::string& prefix) const;

	/**
	 * Return a copy of this ParameterData, with the string prefix removed from the beginning of all key values.
	 * If any key value in this object does not start with the given prefix, do not include it in the returned object.
	 * This is intended to filter out the parameters of a particular instance of a group of objects.
	 * The resulting ParameterData object will include an empty string parameter if a key exactly matches the prefix.
	 * 
	 * @param prefix
	 * @return
	 */
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
	 */
	double getValue(const std::string& key) const;
	/**
	 * Returns the double-precision value of the given parameter key in internal
	 * units. Only in the case when units were not specified in the file, will
	 * the defaultUnit parameter be used. If the key is not present or if the
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
	 * makes no change to the parameter database.  If the supplied units
	 * are unspecified, then the units in the database are used to interpret the value
	 * given.  <p>
	 * 
	 * Examples of valid strings include:
	 * <ul>
	 * <li> a = true
	 * <li> b = hello everyone!
	 * <li> c = 10 [NM]
	 * </ul>
	 * 
	 * @param s the given string to parse
	 * @return true if the database was updated, false otherwise.  The database may not
	 * be updated because of an invalid parameter string was given, or incompatible units, etc.
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
	/** Associates a boolean value with a parameter key. 
	 * 
	 * @return true if the database was updated, false otherwise.  The database may not
	 * be updated because of incompatible units, etc.
	 * */
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
	/** Associates a value (in internal units) with a parameter key. How the units in the database
	 * are updated, depends on the value of the setPreserveUnits() parameter.
	 * 
	 * @param key   the name of the parameter
	 * @param value the value of the parameter in INTERNAL units
	 * @param units the typical units of the value (but no conversion takes place)
	 * @return true if the database was updated, false otherwise.  The database may not
	 * be updated because of incompatible units, etc.
	 */
	bool setInternal(const std::string& key, double value, const std::string& units);
	/** Associates a value (in internal units) with a parameter key. How the units in the database
	 * are updated, depends on the value of the setPreserveUnits() parameter.
	 * 
	 * @param key   the name of the parameter
	 * @param value the value of the parameter in INTERNAL units
	 * @param units the typical units of the value (but no conversion takes place)
	 * @param prec  precision 
	 * @return true if the database was updated, false otherwise.  The database may not
	 * be updated because of incompatible units, etc.
	 */
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
	/**
	 * Copy a ParameterData object into this object.  That is, A.copy(B,true) means A &lt;--- B.
	 * @param p source ParameterData
	 * @param overwrite if a parameter key exists in both this object and p, if overwrite is true then p's value will be used, otherwise this object's value will be used
	 */
	void copy(ParameterData p, bool overwrite);

	/**
	 * Remove the given key from this database.  If the key does not exist, do nothing.
	 * @param key
	 */
	void remove(const std::string& key);

	/**
	 * Remove all keys in the list from this database, if they exist.
	 */
	void removeAll(const std::vector<std::string>& key);


	/**
	 * Return this ParameterData as a single line string that can subsequently be parsed by parseLine()
	 * @param separator Unique, non-empty string to separate each entry.  If left empty or null, the defaultEntrySeparator is used.
	 * @return Single-line string representation of this ParameterData object, or the null object if the given pattern appears in any parameter definition, including whitespace.
	 */
	std::string toParameterList(const std::string& separator) const;
	/**
	 * Read in a set of parameters as created by toParameterList() with the same separator.
	 * @param separator string used to separate definitions.  If blank or null, use defaultEntrySeparator instead.
	 * @param line String to be read.
	 * @return true if string parsed successfully, false if an error occurred.  If this returns false, one or more entries were not added to the database.
	 */
	bool parseParameterList(const std::string& separator, std::string line);

	/**
	 * Returns a string listing all parameters and their (original string) values
	 */
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
	/**
	 * Parse a string, if it is a valid parameter, then add it to the database and return true, else false.
	 * @param str the string to parse
	 * @return true if the database was updated, false otherwise.  The database may not
	 * be updated because of an invalid parameter string was given, or incompatible units, etc.
	 */
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
