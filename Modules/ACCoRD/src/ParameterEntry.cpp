/*
 * Copyright (c) 2014-2018 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#include "ParameterEntry.h"
#include "Util.h"
#include "Units.h"
#include "format.h"
#include "string_util.h"
#include <sstream>

namespace larcfm {

long ParameterEntry::count = 0;

ParameterEntry::ParameterEntry() :
	sval(""),
	dval(NaN),
	units("unspecified"),
	bval(false),
	comment(""),
	order(count++) {
}

ParameterEntry::~ParameterEntry() {
}

ParameterEntry::ParameterEntry(const std::string& s, double d, const std::string& u,
		bool b, const std::string& msg) :
	sval(s),
	dval(d),
	units(u),
	bval(b),
	comment(msg),
	order(count++) {
}

ParameterEntry::ParameterEntry(const ParameterEntry& entry) : 
	sval(entry.sval),
	dval(entry.dval),
	units(entry.units),
	bval(entry.bval),
	comment(entry.comment),
	order(entry.order) {
}

// Make boolean entry
ParameterEntry ParameterEntry::makeBoolEntry(bool b) {
	return ParameterEntry(b ? "true" : "false",0,
			"unitless", b, "");
}

// New double entry
ParameterEntry ParameterEntry::makeDoubleEntry(double d, const std::string& u, int p) {
	return ParameterEntry(format(u,d,p),d,u,false,"");
}
// New integer entry
ParameterEntry ParameterEntry::makeIntEntry(int i){
	return ParameterEntry(to_string(i),i,"unitless",false,"");
}

std::string ParameterEntry::format(const std::string& u, double d, int p) {
	if (u != "unitless" && u != "unspecified") {
		if (p < 0) {
			return Units::strX(u,d);
		} 	else {
			return Units::str(u,d,p);
		}
	} else {
		if (p < 0) {
//			return std::to_string(d); // not portable
			std::ostringstream oss;
			oss << d << "";
			return oss.str();
		} else {
			return FmPrecision(d,p);
		}
	}
}

  //void ParameterEntry::set_sval() {
  //	// see java note, was truncating things that "looked like numbers" unnecessarily
  //	//sval = format(units,dval,precision);
  //	sval = trimCopy(sval);
  //}




} /* namespace larcfm */
