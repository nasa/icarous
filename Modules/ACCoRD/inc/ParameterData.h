/*
 * Copyright (c) 2014-2020 United States Government as represented by
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
#include "string_util.h"
#include "ParameterEntry.h"
#include "Function.h"
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
 * key values (getKeyList, getListFull) will reflect the capitalization scheme used for the initial assignment to a key.
 * 
 */
class ParameterData {

public:
	static const std::string parenPattern;
	static const std::string defaultEntrySeparator;

	ParameterData();
	static ParameterData make();

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

	/** Number of parameters in this object
	 * 
	 * @return number of parameters
	 */
	int size() const;

	/**
	 * Returns an array of parameter strings encountered, alphabetically ordered
	 */
	std::vector<std::string> getKeyList() const;

	/**
	 * Returns an array of parameter strings encountered, in the order entered
	 */
	std::vector<std::string> getKeyListEntryOrder() const;

	/**
	 * Returns a list of parameter key strings encountered that satisfy the filter.
	 * Note that this will reflect the original capitalization of the keys when they were first stored.
	 *
	 * @return list of parameter key names
	 */
	std::vector<std::string> getKeyListWithFilter(Function<const std::string&,bool>& f) const;

	/**
	 * Returns an array of parameter definition strings encountered.
	 */
	std::vector<std::string> getListFull() const;
	/**
	 * Removes all stored parameters.
	 */
	void clear();
	/**
	 * Returns true if the parameter key was defined.
	 * 
	 * @param key parameter key to check if it is in database
	 * @return true, if parameter is in database
	 */
	bool contains(const std::string& key) const;

	std::vector<std::string> matchList(const std::string& key) const;

	/**
	 * Return a copy of this ParameterData object, with all key values being prepended with the string prefix.
	 * This is intended to create a unique ParameterData object representing the parameters of a particular 
	 * instance (of many) of an object, that can then be collected along with others into a larger ParameterData object representing the containing object.
	 * @param prefix prefix to add to each key
	 * @return copy of ParameterData with changes
	 */
	ParameterData copyWithPrefix(const std::string& prefix) const;

	/**
	 * Return a copy of this ParameterData, with the string prefix removed from the beginning of all key values.
	 * If any key value in this object does not start with the given prefix, do not include it in the returned object.
	 * This is intended to filter out the parameters of a particular instance of a group of objects.
	 * The resulting ParameterData object will include an empty string parameter if a key exactly matches the prefix.
	 * 
	 * @param prefix prefix of each key returned
	 * @return copy of ParameterData with changes
	 */
	ParameterData extractPrefix(const std::string& prefix) const;

	/**
	 * Return a copy of this ParameterData, with all keys starting with the indicated prefix removed.
	 * Only include the key values that do not start with the given prefix (if any).
	 * @param prefix the prefix to look for
	 * @return copy of ParameterData without matching keys
	 */
	ParameterData removeKeysWithPrefix(const std::string& prefix) const;

	/**
	 * Returns the string value of the given parameter key. This may be a
	 * space-delimited list. If the key is not present, return the empty string.
	 * Parameter keys may be case-sensitive.
	 * 
	 * @param key parameter name
	 * @return string representation of parameter
	 */
	std::string getString(const std::string& key) const;

	/**
	 * Returns the double-precision value of the given parameter key in internal
	 * units. If the key is not present or if the value is not a numeral, then
	 * return 0. Parameter keys may be case-sensitive.
	 * 
	 * @param key parameter name
	 * @return value of parameter (internal units)
	 */
	double getValue(const std::string& key) const;
	/**
	 * Returns the double-precision value of the given parameter key in internal
	 * units. Only in the case when units were not specified in the database, will
	 * the defaultUnit parameter be used. If the key is not present or if the
	 * value is not a numeral, then return 0. Parameter keys may be
	 * case-sensitive.
	 * 
	 * @param key name of parameter
	 * @param defaultUnit units to use if no units are in database
	 * @return value of parameter (internal units)
	 */
	double getValue(const std::string& key, const std::string& defaultUnit) const;
	/**
	 * Returns the string representation of the specified unit of the given
	 * parameter key. If the key is not present or no unit was specified, return
	 * "unspecified". Parameter keys may be case-sensitive.
	 * 
	 * @param key name of parameter
	 * @return units of parameter
	 */
	std::string getUnit(const std::string& key) const;
	/**
	 * Returns the Boolean value of the given parameter key. If the key is not
	 * present, or not representation of "true", return the empty string.
	 * Parameter keys may be case-sensitive.
	 * 
	 * @param key name of parameter
	 * @return boolean representation of parameter
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
	 * 
	 * @param key name of parameter
	 * @return integer representation of parameter
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
	 * 
	 * @param key name of parameter
	 * @return long representation of parameter
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
	 * @return true, if the database was updated, false otherwise.  The database may not
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
	/**
	 * Associates a parameter key with a value (both represented as strings).
	 * Examples of string values include:
	 * <ul>
	 * <li> true
	 * <li> hello everyone!
	 * <li> 20
	 * <li> 10 [NM]
	 * </ul>
	 * 
	 * A note on this last example ('10 [NM]').  The value field may include a units 
	 * descriptor in addition to the actual
	 * value.  This is called the supplied units.  If the supplied units
	 * are unspecified (e.g., '20'), then the units in the database for this parameter
	 * are used to interpret the value given.<p>
	 * 
	 * Another note is that the string value stored is exactly what was supplied,
	 * except white space is trimmed off the beginning and end. 
	 * 
	 * @param key name of parameter
	 * @param value string representation of parameter
	 * @return true, if the database was updated, false otherwise.  The database may not
	 * be updated because of an invalid parameter string was given, or incompatible units, etc.
	 */
	bool set(const std::string& key, const char* value);

	/** Associates a boolean value with a parameter key. 
	 * 
	 * @param key name of parameter
	 * @param value boolean representation of parameter
	 * @return true, if the database was updated, false otherwise.  The database may not
	 * be updated because of incompatible units, etc.
	 */
	bool setBool(const std::string& key, bool value);

	/** Associates true value with a parameter key.
	 *
	 * @param key name of parameter
	 * @return true, if the database was updated, false otherwise.  The database may not
	 * be updated because of incompatible units, etc.
	 */
	bool setTrue(const std::string& key);

	/** Associates false value with a parameter key.
	 *
	 * @param key name of parameter
	 * @return true, if the database was updated, false otherwise.  The database may not
	 * be updated because of incompatible units, etc.
	 */
	bool setFalse(const std::string& key);

	/** Associates an integer value with a parameter key.  Integer values always have
	 * the unit of 'unitless' 
	 * 
	 * @param key name of parameter
	 * @param value integer representation of parameter
	 * @return true, if the database was updated, false otherwise.  The database may not
	 * be updated because of incompatible units, etc.
	 */
	bool setInt(const std::string& key, int value);

	/** Associates a value (in the given units) with a parameter key. If the supplied units
	 * are "unspecified," then the units in the database are used to interpret the value
	 * given.  How the units in the database
	 * are updated depends on the value of the setPreserveUnits() parameter.
	 *
	 *
	 * @param key    the name of the parameter
	 * @param value  the value of the parameter in EXTERNAL units
	 * @param units  the units of the given parameter
	 * @return true, if the database was updated, false otherwise.  The database may not
	 * be updated because of incompatible units, etc.
	 */
	bool set(const std::string& key, double value, const std::string& unit);

	/** Associates a value (in internal units) with a parameter key. How the units in the database
	 * are updated, depends on the value of the setPreserveUnits() parameter.
	 * 
	 * @param key   the name of the parameter
	 * @param value the value of the parameter in INTERNAL units
	 * @param units the typical units of the value (but no conversion takes place, if "unspecified" any old value is preserved)
	 * @return true, if the database was updated, false otherwise.  The database may not
	 * be updated because of incompatible units, etc.
	 */
	bool setInternal(const std::string& key, double value, const std::string& units);

	/** Associates a value (in internal units) with a parameter key. How the units in the database
	 * are updated, depends on the value of the setPreserveUnits() parameter.
	 * 
	 * @param key   the name of the parameter
	 * @param value the value of the parameter in INTERNAL units
	 * @param units the typical units of the value (but no conversion takes place, if "unspecified" any old value is preserved)
	 * @param prec  number of digits of precision to represent this value as a string 
	 * @return true, if the database was updated, false otherwise.  The database may not
	 * be updated because of incompatible units, etc.
	 */
	bool setInternal(const std::string& key, double value, const std::string& units, int prec);

	/** 
	 * Updates the unit for an existing entry. This ignores the setPreservedUnits() flag. 
	 * You may create a blank entry in order to preemptively store a units value. 
	 *  
	 * @param key name of parameter 
	 * @param unit unit for this parameter 
	 * @return If the entry does not exist or the supplied unit is not recognized, this returns false, 
	 *         otherwise it returns true. 
	 * */
	bool updateUnit(const std::string& key, const std::string& unit); 

	/**
	 * Updates entry's comment
	 *
	 * @param key name of parameter
	 * @param msg the new comment of the parameter
	 * @return If the entry does not exist or the supplied unit is not recognized, this returns false, otherwise it returns true.
	 */
	bool updateComment(const std::string& key, const std::string& msg);


	/**
	 * Return a parameter's comment field.
	 * @param key name of parameter
	 * @return The comment string associated with the entry.  If the entry does not exist, return the empty string.
	 */
	std::string getComment(const std::string& key) const;


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
	 * Copy parameter entries from list of keys 
	 * @param p database
	 * @param plist list of keys
	 * @param overwrite if a parameter key exists in both this object and p, if overwrite is true then p's value will be used, otherwise this object's value will be used
	 */
	void listCopy(const ParameterData& p, const std::vector<std::string>& plist, bool overwrite);

	/**
	 * Copy a ParameterData object into this object.  That is, A.copy(B,true) means A &lt;--- B.
	 * @param p source ParameterData
	 * @param overwrite if a parameter key exists in both this object and p, if overwrite is true then p's value will be used, otherwise this object's value will be used
	 */
	void copy(const ParameterData& p, bool overwrite);

	/**
	 * Remove the given key from this database.  If the key does not exist, do nothing.
	 * @param key key name
	 */
	void remove(const std::string& key);

	/**
	 * Remove all keys in the list from this database, if they exist.
	 */
	void removeAll(const std::vector<std::string>& key);

	/**
	 * Return this ParameterData as a single line string that can subsequently be parsed by parseLine()
	 * @param separator A unique (to the key and value set) character string to separate each entry.  If this is null or the empty string, use the defaultEntrySeparator string instead.
	 * @return Single-line string representation of this ParameterData object, possibly empty, or null if the separator is a substring of any key/value entry.
	 *
	 * Note that the delimiter will be included after each entry, including the last one.
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
	 * Returns true if the stored value for key is likely a boolean
	 * @param key parameter name
	 * @return true if key exists and the value is true/false/t/f, false otherwise
	 */
	bool isBoolean(const std::string& key) const;

	/**
	 * Returns true if the stored value for key is likely a number
	 * @param key parameter name
	 * @return true if key exists and the value is a parsable number
	 */
	bool isNumber(const std::string& key) const;

	/**
	 * Returns true if the stored value for key is likely a string (or list).  Note,
	 * the getString() method will always return a string (assuming a valid key
	 * is provided).  This method returns a more narrow definition of a string,
	 * that is, something that is not a number or a boolean.
	 * @param key parameter name
	 * @return true if key exists and the value is not a parse-able number
	 */
	bool isString(const std::string& key) const;

	/**
	 * Returns a string listing of specified parameters in (original) key/value pairs
	 * 
	 * @param keys list of parameter entries
	 * @return multi-line string
	 */
	std::string listToString(const std::vector<std::string>& keys) const;

	/**
	 * Returns a string listing all parameters in keys 
	 * 
	 * @param keys list of parameter entries
	 * @param separator the separator (e.g., a comma) to separate each key
	 * @return string
	 */
	std::string listToString(const std::vector<std::string>& keys, const std::string& separator) const;

	/**
	 * Returns a multi-line string listing all parameters and their (original string) values
	 */
	std::string toString() const;

	/**
	 * Returns a string listing all parameters
	 * @param separator the separator (e.g., a comma) to separate each key
	 * @return A separated string of all parameters
	 */
	std::string toString(const std::string& separator) const;

	bool equals(const ParameterData& pd) const;

	/**
	 * Compare this ParameterData with another.
	 * Return a string listing all differences in stored values between the two.
	 * @param pd parameter database
	 * @return string listing differences, or the empty string if the contents are the same.
	 */
	std::string diffString(const ParameterData& pd) const;

	/**
	 * Compare this object with a base ParameterData object and return all parameters in this object that are different from parameters in the base object,
	 * either having different values or that are not present in the base.  (Parameters that are only in the base are not included.)  If the objects are
	 * the same, the result will be an empty ParameterData object.
	 * @param base "base" or "default" ParameterData to compare this object to
	 * @return A ParameterData object containing all parameters that are in this object, but not in the base, or that are in both but have different values.  This return may be empty.
	 */	ParameterData delta(const ParameterData& base) const;


	std::vector<int> getListInteger(const std::string& key) const;
	std::vector<double> getListDouble(const std::string& key) const;
	std::vector<std::string> getListString(const std::string& key) const;
	std::vector<bool> getListBool(const std::string& key) const;

	bool set(const std::string& key, const std::vector<int>& list);
	bool set(const std::string& key, const std::vector<double>& list);
	bool set(const std::string& key, const std::vector<std::string>& list);
	bool setListBool(const std::string& key, const std::vector<bool>& list);

private:
	int longestKey() const;
	int longestVal() const;

	/**
	 * Parse a string, if it is a valid parameter, then add it to the database and return true, else false.
	 * @param str the string to parse
	 * @return true if the database was updated, false otherwise.  The database may not
	 * be updated because of an invalid parameter string was given, or incompatible units, etc.
	 */
	bool parse_parameter_string(const std::string& str);

	/**
	 * Put entry in parameter map
	 * @param key
	 * @param entry
	 * @return true, if parameter was added successfully
	 */
	bool putParam(const std::string& key, const std::pair<bool, ParameterEntry>& entry);

	bool putParam(const std::string& key, bool perform_conversion, ParameterEntry& entry);

	std::pair<bool, ParameterEntry> parse_parameter_value(const std::string& value);

	static std::vector<std::string> stringList(const std::string& instring);
	static std::vector<int> intList(const std::string& instring);
	static std::vector<double> doubleList(const std::string& instring);
	static std::vector<bool> boolList(const std::string& instring);


	bool preserveUnits;
	bool unitCompatibility;
	std::string listPatternStr;
	typedef std::map<std::string, ParameterEntry, stringCaseInsensitive> paramtype;
	paramtype parameters;

	class comp_order {
	public:
		const paramtype *params;
		comp_order(const paramtype *p) {
			params = p;
		}
		bool operator() (const std::string& lhs, const std::string& rhs) const
		{
			paramtype::const_iterator p1 = params->find(lhs);
			paramtype::const_iterator p2 = params->find(rhs);
			long n1 = p1->second.order;
			long n2 = p2->second.order;
			return n1<n2;
		}
	} ;


};

}

#endif
