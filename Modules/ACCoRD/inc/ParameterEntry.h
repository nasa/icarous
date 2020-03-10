/*
 * Copyright (c) 2014-2019 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#ifndef PARAMETERENTRY_H_
#define PARAMETERENTRY_H_

#include <string>

namespace larcfm {

class ParameterEntry {
public:
	std::string  sval; // String value
	double       dval; // Double value (internal units)
	std::string  units; // Unit
	bool         bval; // Boolean value
	std::string  comment; // Comment for printing
	long		 order; // entry ordering

	ParameterEntry();
	virtual ~ParameterEntry();
	ParameterEntry(const std::string& s, double d, const std::string& u, bool b, const std::string& msg);
	ParameterEntry(const ParameterEntry& entry);

	// Make bool entry
	static ParameterEntry makeBoolEntry(bool b);

	// New double entry
	static ParameterEntry makeDoubleEntry(double d, const std::string& u, int p);

	// New integer entry
	static ParameterEntry makeIntEntry(int i);

	//void set_sval();

private:
	static std::string format(const std::string& u, double d, int p);
	static long count;

};

} /* namespace larcfm */

#endif /* PARAMETERENTRY_H_ */
