/* 
 * PlanReader
 *
 * Contact: George Hagen
 *
 * Copyright (c) 2011-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 *
 */

#ifndef PLANREADER_H
#define PLANREADER_H

#include "ErrorLog.h"
#include "ErrorReporter.h"
#include "SeparatedInput.h"
#include "ParameterReader.h"
#include "ParameterProvider.h"
#include "Plan.h"
#include <string>
#include <vector>

namespace larcfm {

/**
 * This reads in and stores a set of plans (and parameters) from a text file.
 * Plan files consist of comma or space-separated values, with one point per line.
 * Required columns include either:<br>
 * Aircraft name, x-position [nmi], y-position [nmi], z-position [ft], time [s]<br>
 * or<br>
 * Aircraft name, latitude [deg], longitude [deg], altitude [ft], time [s]<p>
 *
 * Optional columns include point mutability and point label, both with [unspecified] units.  Even if these columns are defined, 
 * these two values are optional for any given line.<p>
 *
 * It is necessary to include a header line that defines the column ordering.  The column definitions are not case sensitive.
 * There is also an optional header line, immediately following the column definition, that defines the unit type for each
 * column (the defaults are listed above).<p>
 *
 * If points are consecutive for the same aircraft, subsequent name fields may be replaced with a double quotation mark (").
 * Assume the aircraft name is case sensitive, so US54A != Us54a != us54a.<p>
 *
 * Any empty line or any line starting with a hash sign (#) is ignored.<p>
 *
 * Files may also include parameter definitions prior to other data.  Parameter definitions are of the form <key> = <value>,
 * one per line, where <key> is a case-insensitive alphanumeric word and <value> is either a numeral or string.  The <value> 
 * may include a unit, such as "dist = 50 [m]".  Note that parameters require a space on both sides of the equals sign.<p>
 *
 * Parameters can be interpreted as double values, strings, or Boolean values, and the user is required to know which parameter is
 * interpreted as which type.<p>
 * 
 * If the optional parameter "filetype" is specified, its value must be "plan" or "trajectory" (no quotes) for this reader to accept the 
 * file without error.<p>
 * 
 * Plan "note" information is to be stored with a parameter named ID_note, where ID is the name of the plan in question.
 * 
 */
class PlanReader : public ErrorReporter, public ParameterReader, public ParameterProvider {
private:

	// we store the heading indices in the following order:
	enum {
		NAME,
		LAT_SX,
		LON_SY,
		SZ,
		TIME,
		LABEL,
		TYPE,
		TCP_TRK,
		TCP_GS,
		TCP_VS,
		ACC_TRK,
		ACC_GS,
		ACC_VS,
		TRK,
		GS,
		VS,
		SRC_LAT_SX,
		SRC_LON_SY,
		SRC_ALT,
		SRC_TIME,
		RADIUS
	};

	virtual void loadfile();

protected:

	mutable ErrorLog error;
	SeparatedInput input;
	std::vector<Plan> plans;
//	bool hasRead;
//	bool latlon;
	bool clock;
	std::vector<int> head;

	int altHeadings(const std::string& s1, const std::string& s2, const std::string& s3, const std::string& s4) const;
	int altHeadings(const std::string& s1, const std::string& s2, const std::string& s3) const;
	int altHeadings(const std::string& s1, const std::string& s2) const;
	double getClock(const std::string& s) const;

	int planNameIndex(const std::string& name) const;

public:

	/** Create a new PlanReader */
	PlanReader();

	/** Create a new PlanReader reading the specified file. */
	virtual void open(const std::string& filename);
	virtual void open(std::istream* input);

	/** Convert a NavPoint to a TCP based on a string qualifier */
	static NavPoint setTcpType(const NavPoint& n, const std::string& s);


	/** Return the number of plans in the file */
	int size() const;
	/** Returns the i-th plan in the file */
	Plan getPlan(int i) const;


	ParameterData getParameters() const;
	void updateParameterData(ParameterData& p) const;
	ParameterData& getParametersRef();

	// ErrorReporter Interface Methods

	bool hasError() const {
		return error.hasError() || input.hasError();
	}
	bool hasMessage() const {
		return error.hasMessage() || input.hasMessage();
	}
	std::string getMessage() {
		return error.getMessage() + input.getMessage();
	}
	std::string getMessageNoClear() const {
		return error.getMessageNoClear() + input.getMessageNoClear();
	}


};

}

#endif //PLANREADER_H
