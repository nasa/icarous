/* 
 * PolyReader
 *
 * Authors: George Hagen
 *
 * Copyright (c) 2011-2017 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
#ifndef POLYREADER_H
#define POLYREADER_H

#include "ErrorLog.h"
#include "ErrorReporter.h"
#include "ParameterProvider.h"
#include "ParameterReader.h"
#include "SeparatedInput.h"
#include "Plan.h"
#include "PolyPath.h"
#include <string>
#include <vector>

namespace larcfm {

/**
 * This reads in and stores both Plans and PolyPaths (and parameters) from a file
 * 
 * Plan files consist of comma or space-separated values, with one point per line.
 * Required columns include either:
 * Aircraft name, x-position [nmi], y-position [nmi], z-position [ft], time [s]
 * or
 * Aircraft name, latitude [deg], longitude [deg], altitude [ft], time [s]
 * 
 * Polygon files also include a required second altitude column [ft] (e.g. alt2).
 * 
 * Any given line may contain either type of information, but all lines describing a single entity (either aircraft or polygon)
 * must be located together, and without time decreasing -- new entities are detected by a change in the name field or if
 * (time of entry n) < (time of entry n+1).  The type of the entry is defined by the presence or lack of a second 
 * altitude column value (polys have it, plans don't).
 *
 * Optional columns include point mutability and point label, both with [unspecified] units.  Even if these columns are defined, 
 * these two values are optional for any given line.  Columns without values may be left blank (if comma-delimited) or indicated
 * by a single dash (-).
 *
 * It is necessary to include a header line that defines the column ordering.  The column definitions are not case sensitive.
 * There is also an optional header line, immediately following the column definition, that defines the unit type for each
 * column (the defaults are listed above).
 *
 * If points are consecutive for the same aircraft, subsequent name fields may be replaced with a double quotation mark (").
 * The aircraft name is case sensitive, so US54A != Us54a != us54a.
 *
 * Any empty line or any line starting with a hash sign (#) is ignored.
 *
 * Files may also include parameter definitions prior to other data.  Parameter definitions are of the form <key> = <value>,
 * one per line, where <key> is a case-insensitive alphanumeric word and <value> is either a numeral or string.  The <value> 
 * may include a unit, such as "dist = 50 [m]".  Note that parameters require a space on either side of the equals sign.
 *
 * Parameters can be interpreted as double values, strings, or Boolean values, and the user is required to know which parameter is
 * interpreted as which type.
 * 
 * If the optional parameter "filetype" is specified, its value must be "plan", "trajectory", "poly", or "plan+poly" (no quotes)
 * for this reader to accept the file without error.
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
		RADIUS,
		CENTER_LAT_SX,
		CENTER_LON_SY,
		CENTER_ALT,
		SZ2,
		ENDTIME
	};

	enum {
		POLY,
		UNKNOWN,
		PLAN
	};

	std::vector<PolyPath> paths;
	std::vector<PolyPath> containment;

	int altHeadings5(const std::string& s1, const std::string& s2, const std::string& s3, const std::string& s4, const std::string& s5) const;
	virtual void loadfile();

protected:
	int pathNameIndex(const std::string& name) const;
	int containmentNameIndex(const std::string& name) const;
	int planNameIndex(const std::string& name) const;

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


public:

	/** Create a new PolyReader. */
	PlanReader();
//	/** Create a new PolyReader based on an already-created SeparatedInput. */
//	PolyReader(SeparatedInput& input);

	virtual void open(const std::string& filename);
	virtual void open(std::istream* input);


	/** Return the number of plans in the file */
	int planSize() const;

	/** Return the number of paths in the file */
	int polySize() const;

	/** Return the number of containment areas in the file */
	int containmentSize() const;

	/** Return the number of plans in the file */
	int size() const;

	/** Returns the i-th plan in the file */
	Plan getPlan(int i) const;

	/** Returns the i-th path in the file */
	PolyPath getPolyPath(int i) const;

	/** Returns the i-th containment polygon in the file */
	PolyPath getContainmentPolygon(int i) const;

	/** Returns the total number of both plans and paths. */
	int combinedSize() const;

	/** Returns a plan or the plan corresponding to a path.  Real plans occur first in the list. 
	 * This should only be used if approximating plans by variable-sized traffic.
	 * 
	 * @param i
	 * @return
	 */
	Plan getCombinedPlan(int i) const;

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
