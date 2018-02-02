/*
 * Copyright (c) 2014-2017 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#include "ParameterData.h"
#include "Units.h"
#include "ErrorLog.h"
#include "ErrorReporter.h"
#include "format.h"
#include "string_util.h"
#include "Constants.h"
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <stdexcept>
#include <algorithm>

namespace larcfm {

const std::string ParameterData::parenPattern = "[()]+";
const std::string ParameterData::defaultEntrySeparator = "?";

ParameterData::ParameterData() {
	caseSensitive = true;
	preserveUnits = false;
	unitCompatibility = true;
	parameters = paramtype();
	patternStr = Constants::wsPatternBaseNoRegex;
}

ParameterData ParameterData::copyWithPrefix(const std::string& prefix) const {
	ParameterData p;
	p.preserveUnits = preserveUnits;
	p.unitCompatibility = unitCompatibility;
	p.patternStr = patternStr;
	paramtype::const_iterator pos;
	for (pos = parameters.begin(); pos != parameters.end(); ++pos) {
		std::string key = pos->first;
		p.parameters[prefix+key] = pos->second;
	}
	return p;
}

ParameterData ParameterData::extractPrefix(const std::string& prefix) const {
	std::string prefixlc = toLowerCase(prefix);
	ParameterData p;
	p.preserveUnits = preserveUnits;
	p.unitCompatibility = unitCompatibility;
	p.patternStr = patternStr;
	for (paramtype::const_iterator pos = parameters.begin(); pos != parameters.end(); ++pos) {
		std::string key = pos->first;
		std::string keylc = toLowerCase(key);
		if (keylc.find(prefixlc) == 0) {
			p.parameters[key.substr(prefix.length())] = pos->second;
		}
	}
	return p;
}

bool ParameterData::isCaseSensitive() {
	return caseSensitive;
}

void ParameterData::setCaseSensitive(bool v) {
	caseSensitive = v;
}

bool ParameterData::isPreserveUnits() {
	return preserveUnits;
}

void ParameterData::setPreserveUnits(bool v) {
	preserveUnits = v;
}

bool ParameterData::isUnitCompatibility() {
	return unitCompatibility;
}
void ParameterData::setUnitCompatibility(bool v) {
	unitCompatibility = v;
}


int ParameterData::size() const {
	return parameters.size();
}

std::vector<std::string> ParameterData::getList() const {
	paramtype::const_iterator pos;
	std::vector<std::string> v;
	// TODO: check for empty entries in this list
	for (pos = parameters.begin(); pos != parameters.end(); ++pos) {
		v.push_back(pos->first);
	}
	return v;
}

std::vector<std::string> ParameterData::getListFull() const {
	std::vector<std::string> list = getList();
	for (int i = 0; i < (int) list.size(); i++) {
		std::string s = list[i];
		list[i] = list[i] + " = " + getString(s);
	}
	return list;
}

void ParameterData::clear() {
	parameters.clear();
}

bool ParameterData::contains(const std::string& key) const {
	return parameters.find(key) != parameters.end();
}

std::vector<std::string> ParameterData::matchList(const std::string& substr2) const {
	std::vector<std::string> ret;
	std::vector<std::string> plist = getList();
	for (int i = 0; i < (int) plist.size(); i++) {
		std::string p = plist[i];
		if (p.find(substr2) != std::string::npos)  // checking for a substring
			ret.push_back(p);
	}
	return ret;
}

std::string ParameterData::getString(const std::string& key) const {
	paramtype::const_iterator q = parameters.find(key);
	if (q == parameters.end()) {
		return "";
	} else {
		return q->second.sval;
	}
}

double ParameterData::getValue(const std::string& key) const {
	paramtype::const_iterator q = parameters.find(key);
	if (q == parameters.end()) {
		return 0.0;
	} else {
		return q->second.dval;
	}
}

double ParameterData::getValue(const std::string& key,const std::string& defaultUnit) const {
	return Units::fromInternal(defaultUnit, getUnit(key), getValue(key));
}

std::string ParameterData::getUnit(const std::string& key) const {
	paramtype::const_iterator q = parameters.find(key);
	if (q == parameters.end()) {
		return "unspecified";
	} else {
		return q->second.units;
	}
}

bool ParameterData::getBool(const std::string& key) const {
	paramtype::const_iterator q = parameters.find(key);
	if (q == parameters.end()) {
		return false;
	} else {
		return q->second.bval;
	}
}

int ParameterData::getInt(const std::string& key) const {
	return (int) getValue(key);
}

long ParameterData::getLong(const std::string& key) const {
	return (long) getValue(key);
}

bool ParameterData::parse_parameter_string(const std::string& str) {
	int loc = str.find('=');
	if (loc > 0) {
		std::string id(substring(str,0,loc));
		trim(id);
		if (id.length() == 0) {
			return false;
		}
		std::string value(substring(str,loc+1));
		trim(value);
		return putParam(id, parse_parameter_value(value));
	} else {
		return false;
	}
}

bool ParameterData::putParam(const std::string& ikey, const std::pair<bool, ParameterEntry>& entry) {
	std::string key(ikey);
	bool perform_conversion = entry.first;
	ParameterEntry newEntry = entry.second;

	bool compatible = true;
	if (contains(key)) {
		ParameterEntry oldEntry = parameters[key];
		if (!Units::isCompatible(newEntry.units,oldEntry.units)) {
			compatible = false;
		} else {
			if (newEntry.units == "unspecified") {
				if (perform_conversion) {
					std::string units = oldEntry.units;
					double convert = Units::from(units,newEntry.dval);
					newEntry.dval = convert;
					newEntry.units = units;
					newEntry.set_sval();
				} else {
					newEntry.units = oldEntry.units;
				}
			} else if (isPreserveUnits()) { // newEntry.units != "unspecified"
				if ( oldEntry.units != "unspecified") {
					newEntry.units = oldEntry.units;
				}
			}
		}
	}
	if (compatible || ! unitCompatibility) {
		parameters[key] = newEntry;
		return true;
	} else {
		return false;
	}
}

/** Doesn't do error checking, string should be "trimmed." this should always return a ParameterEntry */
std::pair<bool, ParameterEntry> ParameterData::parse_parameter_value(const std::string& value) {
	bool perform_conversion = true;
	double dbl = Units::parse(value,-123456.78);
	if (dbl == -123456.78) {
		perform_conversion = true;
		dbl = 0.0;
	}
	std::string unit = Units::parseUnits(value);
	bool boolx = false;
	if (equalsIgnoreCase(value, "true") || equalsIgnoreCase(value, "T")) {
		boolx = true;
	}
	ParameterEntry quad(value,dbl,unit,boolx,"",-1);
	return std::pair<bool,ParameterEntry>(perform_conversion,quad);
}

bool ParameterData::set(const std::string& s) {
	return parse_parameter_string(s);
}

bool ParameterData::set(const std::string& key, char* value) {
	return set(key, std::string(value));
}

bool ParameterData::set(const std::string& key, const char* value) {
	return set(key, std::string(value));
}

bool ParameterData::set(const std::string& key, const std::string& value) {
	return parse_parameter_string(key + " = " + value);
}

bool ParameterData::setBool(const std::string& key, bool value) {
	ParameterEntry newEntry = ParameterEntry::makeBoolEntry(value);
	return putParam(key,std::pair<bool, ParameterEntry>(false,newEntry));
}

bool ParameterData::setTrue(const std::string& key) {
	return setBool(key,true);
}

bool ParameterData::setFalse(const std::string& key) {
	return setBool(key,false);
}

bool ParameterData::setInt(const std::string& key, int value) {
	ParameterEntry newEntry = ParameterEntry::makeIntEntry(value);
	return putParam(key,std::pair<bool, ParameterEntry>(false,newEntry));
}

bool ParameterData::set(const std::string& key, double value, const std::string& units) {
	std::string u = Units::clean(units);
	ParameterEntry newEntry = ParameterEntry::makeDoubleEntry(Units::from(u,value),u,Constants::get_output_precision());
	return putParam(key, std::pair<bool, ParameterEntry>(true,newEntry));
}

bool ParameterData::setInternal(const std::string& key, double value,
		const std::string& units) {
	return setInternal(key, value, units, Constants::get_output_precision());
}

bool ParameterData::setInternal(const std::string& key, double value,
		const std::string& units, int prec) {
	std::string u = Units::clean(units);
	ParameterEntry newEntry = ParameterEntry::makeDoubleEntry(value,u,prec);
	return putParam(key, std::pair<bool, ParameterEntry>(false,newEntry));
}

/**
 * Updates the unit for an entry. This ignores the setPreservedUnits() flag.
 *
 * @param key name of parameter
 * @param unit unit for this parameter
 * @return If the entry does not exist or the supplied unit is not recognized, this returns false, otherwise it returns true.
 */
bool ParameterData::updateUnit(const std::string& key, const std::string& unit) {
	paramtype::iterator entry = parameters.find(key);
	if (Units::isUnit(unit) && entry != parameters.end() &&
			Units::isCompatible(entry->second.units,unit) &&
			unit !="unitless" && unit != "unspecified") {
		entry->second.units = unit;
		entry->second.set_sval();
		return true;
	} else {
		return false;
	}
}

/**
 * Updates entry's comment
 *
 * @param key name of parameter
 * @param msg the new comment of the parameter
 * @return If the entry does not exist or the supplied unit is not recognized, this returns false, otherwise it returns true.
 */
bool ParameterData::updateComment(const std::string& key, const std::string& msg) {
	paramtype::iterator entry = parameters.find(key);
	if (entry != parameters.end()) {
		entry->second.comment = msg;
		return true;
	} else {
		return false;
	}
}

/**
 * Updates entry's string
 *
 * @param key name of parameter
 * @param str the new string of the parameter
 * @return If the entry does not exist or the supplied unit is not recognized, this returns false, otherwise it returns true.
 */
bool ParameterData::updateStr(const std::string& key, const std::string& str) {
	paramtype::iterator entry = parameters.find(key);
	if (entry != parameters.end()) {
		entry->second.sval = str;
		entry->second.dval = 0;
		entry->second.bval = false;
		entry->second.units = "unitless";
		return true;
	} else {
		return false;
	}
}

/** Updates entry's double value
 *
 * @param key the name of the parameter
 * @param val the new double value of the parameter in INTERNAL units
 * @return If the entry does not exist or the supplied unit is not recognized, this returns false, otherwise it returns true.
 */
bool ParameterData::updateDouble(const std::string& key, double val) {
	paramtype::iterator entry = parameters.find(key);
	if (entry != parameters.end()) {
		entry->second.dval = val;
		if (entry->second.precision == 0) {
			entry->second.precision = -1;
		}
		entry->second.set_sval();
		return true;
	} else {
		return false;
	}
}

bool ParameterData::updateBool(const std::string& key, bool val) {
	paramtype::iterator entry = parameters.find(key);
	if (entry != parameters.end()) {
		entry->second.sval = val ? "true" : "false";
		entry->second.bval = val;
		return true;
	} else {
		return false;
	}
}

bool ParameterData::updateInt(const std::string& key, int val) {
	paramtype::iterator entry = parameters.find(key);
	if (entry != parameters.end()) {
		entry->second.precision = 0;
		entry->second.dval = val;
		entry->second.set_sval();
		return true;
	} else {
		return false;
	}
}

bool ParameterData::updatePrecision(const std::string& key, int p) {
	paramtype::iterator entry = parameters.find(key);
	if (entry != parameters.end()) {
		entry->second.precision = p;
		entry->second.set_sval();
		return true;
	} else {
		return false;
	}
}

std::vector<std::string> ParameterData::unrecognizedParameters(
		std::vector<std::string> v) const {
	std::vector<std::string> p = getList();
	std::vector<std::string>::iterator pos;
	unsigned int i = 0;
	while (i < v.size()) {
		pos = find(p.begin(), p.end(), v[i]);
		if (pos != p.end()) {
			p.erase(pos);
		} else {
			i++;
		}
	}
	return p;
}

std::vector<std::string> ParameterData::validateParameters(
		std::vector<std::string> c) {
	c.push_back("filetype");
	return unrecognizedParameters(c);
}

void ParameterData::listCopy(const ParameterData& p, const std::vector<std::string>& plist, bool overwrite) {
	for (int i = 0; i < (int) plist.size(); i++) {
		std::string key = plist[i];
		if (overwrite || ! contains(key)) {
			set(key, p.getString(key));
		}
	}
}

void ParameterData::copy(const ParameterData& p, bool overwrite) {
	listCopy(p,p.getList(),overwrite);
}

/**
 * Remove the given key from this database.  If the key does not exist, do nothing.
 * @param key
 */
void ParameterData::remove(const std::string& key) {
	paramtype::iterator pos = parameters.find(key);
	if (pos != parameters.end()) {
		parameters.erase(pos);
	}
}

void ParameterData::removeAll(const std::vector<std::string>& keys) {
	for (int i = 0; i < (int)keys.size(); i++) {
		remove(keys[i]);
	}
}

std::string ParameterData::toParameterList(const std::string& separator) const {
	std::string sep = separator;
	if (separator.length() < 1) {
		sep = defaultEntrySeparator;
	}
	std::string ret = "";
	std::vector<std::string> list = getListFull();
	for (int i = 0; i < (int) list.size(); i++) {
		std::string def = list[i];
		if (def.find(sep) == std::string::npos) {
			return "";
		}
		ret += def+sep;
	}
	return ret;
}

bool ParameterData::parseParameterList(const std::string& separator,  std::string line) {
	std::string sep = separator;
	if (separator.length() < 1) {
		sep = defaultEntrySeparator;
	}
	bool status = true;
	trim(line);
	std::vector<std::string> s = split(line, sep);
	for (int i = 0; i < (int) s.size(); i++) {
		std::string def = s[i];
		if (def.length() > 3) { // minimum possible length for a parameter definition
			status = status && set(def);
		}
	}
	return status;
}

std::vector<std::string> ParameterData::getListString(const std::string& key) const {
	paramtype::const_iterator q =
			parameters.find(key);

	if (q == parameters.end()) {
		return std::vector<std::string>();
	} else {
		return stringList(q->second.sval);
	}
}

std::vector<int> ParameterData::getListInteger(const std::string& key) const {
	paramtype::const_iterator q =
			parameters.find(key);

	if (q == parameters.end()) {
		return std::vector<int>();
	} else {
		return intList(q->second.sval);
	}
}

std::vector<double> ParameterData::getListDouble(const std::string& key) const {
	paramtype::const_iterator q =
			parameters.find(key);

	if (q == parameters.end()) {
		return std::vector<double>();
	} else {
		return doubleList(q->second.sval);
	}
}

std::vector<bool> ParameterData::getListBool(const std::string& key) const {
	paramtype::const_iterator q =
			parameters.find(key);

	if (q == parameters.end()) {
		return std::vector<bool>();
	} else {
		return boolList(q->second.sval);
	}
}

bool ParameterData::set(const std::string& key, const std::vector<int>& list) {
	std:: string s = "";
	if (list.size() > 0) {
		s = ""+Fm0(list[0]);
		for (int i = 0; i < (int) list.size(); i++) {
			s = s + ","+Fm0(list[i]);
		}
	}
	return set(key,s);
}

bool ParameterData::set(const std::string& key, const std::vector<double>& list) {
	std:: string s = "";
	if (list.size() > 0) {
		s = ""+Fm8(list[0]);
		for (int i = 0; i < (int) list.size(); i++) {
			s = s + ","+Fm8(list[i]);
		}
	}
	return set(key,s);
}

bool ParameterData::set(const std::string& key, const std::vector<std::string>& list) {
	std:: string s = "";
	if (list.size() > 0) {
		s = ""+list[0];
		for (int i = 0; i < (int) list.size(); i++) {
			s = s + ","+list[i];
		}
	}
	return set(key,s);
}

bool ParameterData::setListBool(const std::string& key, const std::vector<bool>& list) {
	std:: string s = "";
	if (list.size() > 0) {
		s = (list[0] ? "true" : "false");
		for (int i = 0; i < (int) list.size(); i++) {
			s = s + ","+(list[i] ? "true" : "false");;
		}
	}
	return set(key,s);
}

/**
 * Returns true if the stored value for key is likely a boolean
 * @param key parameter name
 * @return true if key exists and the value is true/false/t/f, false otherwise
 */
bool ParameterData::isBoolean(const std::string& key) const {
	if (parameters.find(key) == parameters.end()) {
		return false;
	} else {
		return Util::is_boolean(parameters.find(key)->second.sval);
	}
}

/**
 * Returns true if the stored value for key is likely a number
 * @param key parameter name
 * @return true if key exists and the value is a parsable number
 */
bool ParameterData::isNumber(const std::string& key) const {
	if (parameters.find(key) == parameters.end()) {
		return false;
	} else {
		std::string s = parameters.find(key)->second.sval;
		std::vector<std::string> fields = split(s, patternStr);
		if (fields.size() == 1) {
			return Util::is_double(fields[0]);
		} else if (fields.size() == 2) {
			return Util::is_double(fields[0]) && !Util::is_double(fields[1]); // if both numbers, probably a list
		} else {
			return false; // probably a list
		}
	}
}

/**
 * Returns true if the stored value for key is likely a string (or list).  Note,
 * the getString() method will always return a string (assuming a valid key
 * is provided).  This method returns a more narrow definition of a string,
 * that is, something that is not a number or a boolean.
 * @param key parameter name
 * @return true if key exists and the value is not a parse-able number
 */
bool ParameterData::isString(const std::string& key) const {
	if (parameters.find(key) == parameters.end()) {
		return false;
	} else {
		return !isNumber(key) && !isBoolean(key);
	}
}

std::string ParameterData::listToString(const std::vector<std::string>& keys) const {
	std::string s = "";
	for (int i = 0; i < (int) keys.size(); i++) {
		std::string key = keys[i];
		paramtype::const_iterator val = parameters.find(key);
		if (val != parameters.end()) {
			if (val->second.comment != "") {
				s += "# "+val->second.comment+"\n";
			}
			s += key+" = "+val->second.sval+"\n";
		}
	}
	return s;
}

std::string ParameterData::toString() const {
	std::vector<std::string> keys;
	paramtype::const_iterator pos;
	for (pos = parameters.begin(); pos != parameters.end(); ++pos) {
		keys.push_back(pos->first);
	}
	std::sort(keys.begin(), keys.end());
	return listToString(keys);
}

std::string ParameterData::listToString(const std::vector<std::string>& keys, const std::string& separator) const {
	std::string s = "";
	bool first = true;
	for (int i = 0; i < (int) keys.size(); i++) {
		std::string key = keys[i];
		paramtype::const_iterator val = parameters.find(key);
		if (val != parameters.end()) {
			if (first) {
				first = false;
			} else {
				s += separator;
			}
			s += key+"="+val->second.sval;
		}
	}
	return s;
}

/**
 * Returns a string listing all parameters in keys
 *
 * @param keys list of parameter entries
 * @parame separator
 * @return string
 */
std::string ParameterData::toString(const std::string& separator) const {
	std::vector<std::string> keys;
	paramtype::const_iterator pos;
	for (pos = parameters.begin(); pos != parameters.end(); ++pos) {
		keys.push_back(pos->first);
	}
	std::sort(keys.begin(), keys.end());
	return listToString(keys,separator);
}

bool ParameterData::equals(const ParameterData& pd) const {
	if (parameters.size() != pd.parameters.size()) return false;
	std::vector<std::string> keys = getList();
	for (int i = 0; i < (int) keys.size(); i++) {
		std::string key = keys[i];
		if (!pd.contains(key)) return false;
		if (getString(key) != pd.getString(key)) return false; // this should cover all cases
	}
	return caseSensitive == pd.caseSensitive && preserveUnits == pd.preserveUnits && unitCompatibility == pd.unitCompatibility;
}

std::vector<std::string> ParameterData::stringList(const std::string& instring) {
	std::string s = instring;
	larcfm::trim(s);
	//fpln("q1 "+s);
	std::vector<std::string> ss = larcfm::split_empty(s, Constants::separatorPattern);
	//fpln("q2 "+list2str(ss,", "));
	std::vector<std::string> list;
	for (int i = 0; i < (int) ss.size(); i++) {
		larcfm::trim(ss[i]);
		list.push_back(ss[i]);
	}
	return list;
}

std::vector<int> ParameterData::intList(const std::string& instring) {
	std::string s = instring;
	larcfm::trim(s);
	std::vector<std::string> ss = ParameterData::stringList(s);
	std::vector<int> list;
	for (int i = 0; i < (int) ss.size(); i++) {
		list.push_back((int)Util::parse_double(ss[i]));
	}
	return list;
}

std::vector<double> ParameterData::doubleList(const std::string& instring) {
	std::string s = instring;
	std::vector<std::string> ss = ParameterData::stringList(s);
	std::vector<double> list;
	for (int i = 0; i < (int) ss.size(); i++) {
		list.push_back(Units::parse(ss[i]));
	}
	return list;
}

std::vector<bool> ParameterData::boolList(const std::string& instring) {
	std::string s = instring;
	larcfm::trim(s);
	std::vector<std::string> ss = ParameterData::stringList(s);
	std::vector<bool> list;
	for (int i = 0; i < (int) ss.size(); i++) {
		list.push_back(equalsIgnoreCase(ss[i], "true") || equalsIgnoreCase(ss[i], "T"));
	}
	return list;
}

}
