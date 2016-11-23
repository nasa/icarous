/*
 * Copyright (c) 2014-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#include "ParameterData.h"
#include "Units.h"
#include "ErrorLog.h"
#include "ErrorReporter.h"
#include "format.h"
#include "Quad.h"
#include "string_util.h"
#include "Constants.h"
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <stdexcept>
#include <algorithm>

namespace larcfm {

//using string_util;
using std::map;

const std::string ParameterData::parenPattern = "[()]+";
const std::string ParameterData::defaultEntrySeparator = "?";



ParameterData::ParameterData() {
	caseSensitive = true;
	preserveUnits = false;
	unitCompatibility = true;
	parameters = map<std::string, Quad<std::string, double, std::string, bool>, stringCaseInsensitive >();
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
	for (map<std::string, Quad<std::string, double, std::string, bool>, stringCaseInsensitive >::const_iterator pos = parameters.begin(); pos != parameters.end(); ++pos) {
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


int ParameterData::size() {
	return parameters.size();
}

std::vector<std::string> ParameterData::getList() const {
	paramtype::const_iterator pos;  //aaaarrrrggggg!!!
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
		//list[i] = list[i] + " = " + this->getString(list[i]);
		list[i] = list[i] + " = " + getString(s);
	}
	return list;
}

void ParameterData::clear() {
	parameters.clear();
}

bool ParameterData::contains(const std::string& key) const {
//	std::string key;
//	if (!caseSensitive) {
//		key = toLowerCase(str);
//	} else {
//		key = str;
//	}
	return parameters.find(key) != parameters.end();
}

std::vector<std::string> ParameterData::matchList(const std::string& substr2) const {
//	std::string substr2;
//	if (!caseSensitive) {
//		substr2 = toLowerCase(substr);
//	} else {
//		substr2 = substr;
//	}
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
//	std::string key;
//	if (!caseSensitive) {
//		key = toLowerCase(str);
//	} else {
//		key = str;
//	}
	std::map<std::string, Quad<std::string, double, std::string, bool>, stringCaseInsensitive >::const_iterator q =
			parameters.find(key);

	if (q == parameters.end()) {
		return "";
	} else {
		return q->second.getFirst();
	}
}

double ParameterData::getValue(const std::string& key) const {
//	std::string key;
//	if (!caseSensitive) {
//		key = toLowerCase(str);
//	} else {
//		key = str;
//	}
	std::map<std::string, Quad<std::string, double, std::string, bool>, stringCaseInsensitive >::const_iterator q =
			parameters.find(key);

	if (q == parameters.end()) {
		return 0.0;
	} else {
		return q->second.getSecond();
	}
}

double ParameterData::getValue(const std::string& key,
		const std::string& defaultUnit) const {
	double val = getValue(key);
	//fpln("getValue_default "+key+" "+getUnit(key)+" "+defaultUnit);
	if (getUnit(key) == "unspecified") {
		val = Units::from(defaultUnit, val);
	}
	return val;
}

std::string ParameterData::getUnit(const std::string& key) const {
//	std::string key;
//	if (!caseSensitive) {
//		key = toLowerCase(str);
//	} else {
//		key = str;
//	}
	std::map<std::string, Quad<std::string, double, std::string, bool>, stringCaseInsensitive >::const_iterator q =
			parameters.find(key);

	if (q == parameters.end()) {
		return "unspecified";
	} else {
		return q->second.getThird();
	}
}

bool ParameterData::getBool(const std::string& key) const {
//	std::string key;
//	if (!caseSensitive) {
//		key = toLowerCase(str);
//	} else {
//		key = str;
//	}
	std::map<std::string, Quad<std::string, double, std::string, bool>, stringCaseInsensitive >::const_iterator q =
			parameters.find(key);

	if (q == parameters.end()) {
		return false;
	} else {
		return q->second.getFourth();
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
	//f.pln("PD "+str+" XX "+loc);

	if (loc > 0) {
		std::string id(substring(str,0,loc));
		trim(id);
		if (id.length() == 0) {
			return false;
		}
		std::string value(substring(str,loc+1));
		trim(value);
		//parameters[id] = parse_parameter_value(value);
		return putParam(id, parse_parameter_value(value));
	} else {
		return false;
	}
}

//bool ParameterData::parse_parameter_string(const std::string& str) {
//
//	std::vector<std::string> fields = split(str, patternStr);
//
//	// parameter keys are lower case only
//	if (fields.size() >= 3
//			&& (fields[1].compare("=") == 0 || fields[1].compare("==") == 0)
//			&& fields[0].length() > 0) {
//		std::string id = fields[0];
//		if (!caseSensitive)
//			id = toLowerCase(id);
//
//		// string value
//		std::string strv = fields[2];
//		for (unsigned int i = 3; i < fields.size(); i++) {
//			strv += " " + fields[i];
//		} // values become a space-delineated list string
//
//		// Double value
//		double dbl = 0.0;
//		std::string unit = "unspecified";
//		try {
//			dbl = getd(fields[2]);
//			if (fields.size() >= 4) {
//				unit = Units::clean(fields[3]);
//				dbl = Units::from(unit, dbl);
//			}
//		}
//		catch (std::runtime_error e) {
//			dbl = 0.0;  // string is not a number so use a default value
//		}
//
//		// Boolean value
//		bool bval = false;
//		if (equalsIgnoreCase(strv, "true") || equalsIgnoreCase(strv, "T")) {
//			bval = true;
//		}
//		std::string strv2 = str.substr(str.find(fields[2], str.find("=")));
//		trim(strv2);
//		Quad<std::string, double, std::string, bool> quad = Quad<std::string,
//				double, std::string, bool>::make(strv2, dbl, unit, bval);
//		//Quad<string,double,string,bool> quad = Quad<string,double,string,bool>::make(strv, dbl, unit, bval);
//
//		parameters[id] = quad;
//
//		return true;
//	} else {
//		return false;
//	}
//}





//void ParameterData::putParam(std::string ikey, Quad<std::string, double, std::string, bool> newEntry) {
//	std::string key(ikey);
//	if ( ! caseSensitive) {
//		key = toLowerCase(ikey);
//	}
//	if (preserveUnits && contains(key)) {
//		Quad<std::string, double, std::string, bool> oldEntry = parameters[key];
//		if ( oldEntry.third != "unspecified") {
//			newEntry = Quad<std::string, double, std::string, bool>(newEntry.first,newEntry.second,oldEntry.third,newEntry.fourth);
//		}
//	}
//	parameters[key] = newEntry;
//}

bool ParameterData::putParam(const std::string& ikey, const std::pair<bool, Quad<std::string, double, std::string, bool> >& entry) {
	//f.pln("ParameterData.putParam "+key+" "+newEntry);
	std::string key(ikey);
	bool preserve_string = entry.first;
	Quad<std::string, double, std::string, bool> newEntry = entry.second;
//	if ( ! caseSensitive) {
//		key = toLowerCase(key);
//	}

	bool compatible = true;
	if (contains(key)) {
		Quad<std::string, double, std::string, bool> oldEntry = parameters[key];
		if ( ! Units::isCompatible(newEntry.third,oldEntry.third)) {
			compatible = false;
		} else {
			if (newEntry.third == "unspecified" && ! preserve_string) {
				std::string units = oldEntry.third;
				double convert = Units::from(units,newEntry.second);
				newEntry = Quad<std::string, double, std::string, bool>(Units::strX(units,newEntry.second),convert,units,newEntry.fourth);
			} else if (isPreserveUnits()) { // newEntry.third != "unspecified"
				if ( oldEntry.third != "unspecified") {
					newEntry = Quad<std::string, double, std::string, bool>(newEntry.first,newEntry.second,oldEntry.third,newEntry.fourth);
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




//Quad<std::string, double, std::string, bool> ParameterData::parse_parameter_value(const std::string& value) {
//	double dbl = Units::parse(value);
//	std::string unit = Units::parseUnits(value);
//	bool b = toLowerCase(value) == "true" || toLowerCase(value) == "T";
//	Quad<std::string, double, std::string, bool> quad(value,dbl,unit,b);
//	return quad;
//}
/** Doesn't do error checking, string should be "trimmed." this should always return a Quad */
std::pair<bool, Quad<std::string, double, std::string, bool> > ParameterData::parse_parameter_value(const std::string& value) {
	bool preserve_string = false;
	double dbl = Units::parse(value,-123456.78);
	if (dbl == -123456.78) {
		preserve_string = true;
		dbl = 0.0;
	}
	std::string unit = Units::parseUnits(value);
	bool boolx = false;
	if (equalsIgnoreCase(value, "true") || equalsIgnoreCase(value, "T")) {
		boolx = true;
	}
	Quad<std::string, double, std::string, bool> quad = Quad<std::string, double, std::string, bool>(value,dbl,unit,boolx);
	return std::pair<bool, Quad<std::string, double, std::string, bool> >(preserve_string,quad);
}




bool ParameterData::set(const std::string& s) {
	return parse_parameter_string(s);
}

bool ParameterData::set(const std::string& key, char* value) {
	//fpln("char Param set "+key+" "+value);
	return set(key, std::string(value));
}
bool ParameterData::set(const std::string& key, const char* value) {
	//fpln("char Param set "+key+" "+value);
	return set(key, std::string(value));
}
bool ParameterData::set(const std::string& key, const std::string& value) {
	//fpln("Param set "+key+" "+value);
	return parse_parameter_string(key + " = " + value);
}

bool ParameterData::set(const std::string& key, double value,
		const std::string& units) {
	//set(key, Fm12(value) + " [" + units + "]");
	Quad<std::string, double, std::string, bool> newEntry(Fm8(value) + " [" + units +"]",Units::from(units,value),units,false);
	return putParam(key, std::pair<bool, Quad<std::string, double, std::string, bool> >(false,newEntry));
}

bool ParameterData::setInternal(const std::string& key, double value,
		const std::string& units) {
	//set(key, Units::strX(units, value));
  Quad<std::string, double, std::string, bool> newEntry(Units::str(units,value,8),value,units,false);
	return putParam(key, std::pair<bool, Quad<std::string, double, std::string, bool> >(false,newEntry));
}

bool ParameterData::setInternal(const std::string& key, double value,
		const std::string& units, int prec) {
	//set(key, Units::strX(units, value));
  Quad<std::string, double, std::string, bool> newEntry(Units::str(units,value,prec),value,units,false);
	return putParam(key, std::pair<bool, Quad<std::string, double, std::string, bool> >(false,newEntry));
}

bool ParameterData::setTrue(const std::string& key) {
	set(key, "true");
	Quad<std::string, double, std::string, bool> newEntry("true",0.0,"unitless",true);
	return putParam(key, std::pair< bool, Quad<std::string, double, std::string, bool> >(true,newEntry));

}
bool ParameterData::setFalse(const std::string& key) {
	set(key, "false");
	Quad<std::string, double, std::string, bool> newEntry("false",0.0,"unitless",false);
	return putParam(key, std::pair< bool, Quad<std::string, double, std::string, bool> >(true,newEntry));

}

bool ParameterData::setBool(const std::string& key, bool val) {
	if (val) {
		return setTrue(key);
	} else {
		return setFalse(key);
	}
}


std::vector<std::string> ParameterData::unrecognizedParameters(
		std::vector<std::string> v) const {
//	if (!caseSensitive) {
//		ArrayList < String > c2 = new ArrayList<String>(c.size());
//		Iterator < String > it = c.iterator();
//		while (it.hasNext()) {
//			c2.add(it.next().toLowerCase());
//		}
//		c = c2;
//	}
//	ArrayList < String > p = new ArrayList<String>(parameters.keySet());
//	p.removeAll(c);
//	return p.toArray(new String[p.size()]);
//}
//
  std::vector<std::string> p = getList();
  std::vector<std::string>::iterator pos;
  unsigned int i = 0;
  while (i < v.size()) {
//      if (!caseSensitive) {
//      	v[i] = toLowerCase(v[i]);
//      }
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




void ParameterData::copy(ParameterData p, bool overwrite) {
	std::vector<std::string> plist = p.getList();
	for (int i = 0; i < (int) plist.size(); i++) {
		std::string key = plist[i];
		if (overwrite || ! contains(key)) {
			set(key, p.getString(key));
		}
	}
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
	std::map<std::string, Quad<std::string, double, std::string, bool>, stringCaseInsensitive >::const_iterator q =
			parameters.find(key);

	if (q == parameters.end()) {
		return std::vector<std::string>();
	} else {
		return stringList(q->second.getFirst());
	}
}

std::vector<int> ParameterData::getListInteger(const std::string& key) const {
	std::map<std::string, Quad<std::string, double, std::string, bool>, stringCaseInsensitive >::const_iterator q =
			parameters.find(key);

	if (q == parameters.end()) {
		return std::vector<int>();
	} else {
		return intList(q->second.getFirst());
	}
}

std::vector<double> ParameterData::getListDouble(const std::string& key) const {
	std::map<std::string, Quad<std::string, double, std::string, bool>, stringCaseInsensitive >::const_iterator q =
			parameters.find(key);

	if (q == parameters.end()) {
		return std::vector<double>();
	} else {
		return doubleList(q->second.getFirst());
	}
}

std::vector<bool> ParameterData::getListBool(const std::string& key) const {
	std::map<std::string, Quad<std::string, double, std::string, bool>, stringCaseInsensitive >::const_iterator q =
			parameters.find(key);

	if (q == parameters.end()) {
		return std::vector<bool>();
	} else {
		return boolList(q->second.getFirst());
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


std::string ParameterData::toString() const {
	std::string s = "";
	std::vector<std::string> keys;
	paramtype::const_iterator pos;
	for (pos = parameters.begin(); pos != parameters.end(); ++pos) {
		keys.push_back(pos->first);
	}
	std::sort(keys.begin(), keys.end());
	for (int i = 0; i < (int) keys.size(); i++) {
		std::string key = keys[i];
		pos = parameters.find(key);
		std::string val = pos->second.first;
		s = s+key+" = "+val+"\n";
	}
	return s;
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
