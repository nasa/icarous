/*
 * Copyright (c) 2017-2019 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
/*
 * GeneralPlanWriter.h
 *
 *  Created on: Mar 8, 2017
 *      Author: ghagen
 */

#ifndef GENERALPLANWRITER_H
#define GENERALPLANWRITER_H

#include "GeneralPlan.h"
#include "ErrorLog.h"
#include "ErrorReporter.h"
#include "SeparatedOutput.h"
#include "Plan.h"
#include "PolyPath.h"
#include "ParameterData.h"
#include <string>
#include <vector>
#include <iostream>

namespace larcfm {

/**
 * This object writes a set of aircraft states, possibly over time, (and parameters) from a file
 * The Aircraft states are stored in an ArrayList&lt;AircraftState&gt;.
 *
 * State files consist of comma or space-separated values, with one point per line.
 * Required columns include aircraft name, 3 position columns (either x[NM]/y[NM]/z[ft] or 
 * latitude[deg]/longitude[deg]/altitude[ft]) and
 * 3 velocity columns (either vx[knot]/vy[knot]/vz[fpm] or track[deg]/gs[knot]/vs[fpm]).
 *
 * An optional column is time [s].  If it is included, a "history" will be build if an aircraft has more than one entry.
 * If it is not included, only the last entry for an aircraft will be stored.
 *
 * It is necessary to include a header line that defines the column ordering.  The column definitions are not case sensitive.
 * There is also an optional header line, immediately following the column definition, that defines the unit type for each
 * column (the defaults are listed above).
 *
 * If points are consecutive for the same aircraft, subsequent name fields may be replaced with a double quotation mark (&quot;).
 * The aircraft name is case sensitive, so US54A != Us54a != us54a.
 *
 * Any empty line or any line starting with a hash sign (#) is ignored.
 *
 * Files may also include parameter definitions prior to other data.  Parameter definitions are of the form &lt;key&gt; = &lt;value&gt;,
 * one per line, where &lt;key&gt; is a case-insensitive alphanumeric word and &lt;value&gt; is either a numeral or string.  The &lt;value&gt;
 * may include a unit, such as "dist = 50 [m]".  Note that parameters require a space on either side of the equals sign.
 * Note that it is possible to also update the stored parameter values (or store additional ones) through API calls.
 *
 * Parameters can be interpreted as double values, strings, or Boolean values, and the user is required to know which parameter is
 * interpreted as which type.
 *
 * If the optional parameter "filetype" is specified, its value must be "state" or "history" (no quotes) for this reader to accept the 
 * file without error.
 *
 */
class GeneralPlanWriter : ErrorReporter {
private:
	ErrorLog error;
	SeparatedOutput output;
	bool latlon;
	bool trkgsvs;
	bool display_time;
	bool first_line;
	bool display_units;
	bool polygons; // includes polygons (needs extra header)
	bool source;
	bool time2;
	int precision;
	int latLonExtraPrecision;
	int lines_i;
	int num;
	std::string fname;
	PolyPath::PathMode mode;
	bool modedefined;
	std::ostream* fw;


public:
	/** A new GeneralStateWriter. */
	GeneralPlanWriter();

	/** A new GeneralStateWriter based on the given file.
	 * @param filename name of file
	 * */
	void open(const std::string& filename);

	void open(std::ostream* writer);

	void close();

	bool isTrkGsVs() const;

	void setTrkGsVs(bool trkgsvs);

	bool hasPolygons() const;

	/**
	 * Toggle extra polygon headers, true=include (default true)
	 * @param p
	 */
	void setPolygons(bool p);


	int getPrecision() const;

	void setPrecision(int precision);

	int getLatLonExtraPrecision() const;

	void setLatLonExtraPrecision(int precision);

	/** Will the time be added to the file
	 * @return true, if the time is to be output to the file
	 * */
	bool isOutputTime() const;

	/** Should the time be added to the file
	 * @param display_time true, if the time is to be output to the file
	 */
	void setOutputTime(bool display_time);

	/** Will the units be displayed?
	 * @return true, if the units are to be displayed in output
	 * */
	bool isOutputUnits() const;

	/** Should the units be displayed?
	 * @param display_units true, if the units are to be displayed in output
	 * */
	void setOutputUnits(bool display_units);

	/**
	 * Sets the column delimiter to a tab.  This method can only be used before the first "writeState" method.
	 */
	void setColumnDelimiterTab();

	/**
	 * Sets the column delimiter to a comma.  This method can only be used before the first "writeState" method.
	 */
	void setColumnDelimiterComma();

	/**
	 * Sets the column delimiter to a space.  This method can only be used before the first "writeState" method.
	 */
	void setColumnDelimiterSpace();

	/**
	 * Adds a comment line to the file.
	 * @param comment comment string
	 */
	void addComment(const std::string& comment);

	/**
	 * Set parameters.  Use all the parameters in the reader.
	 * @param pr parameters
	 */
	void setParameters(const ParameterData& pr);

	/**
	 * Specify a polygon PathMode.  If this is not set ahead of time, this will default to the first written item's path mode if it is a PolyPath or MORPHING if it is not.
	 * Whatever the stored PathMode, all polygons written will be converted to that type.
	 * @param m path mode
	 */
	void setPolyPathMode(PolyPath::PathMode m);

	/**
	 * Clear any set polygon PathMode.  The PathMode will default to the first written item's path mode if it is a PolyPath or MORPHING if it is not.
	 * Whatever the stored PathMode, all polygons written will be converted to that type.
	 */
	void clearPolyPathMode();

	void setPolyPathParameters(const std::vector<GeneralPlan>& list);

	void setContainmentParameter(const std::string& s);

	void writePlan(const GeneralPlan& gp, double activation_time);

	void writePlan(const GeneralPlan& gp);

	void writePlan(const Plan& p, double t);

	void writePlan(const PolyPath& p, double t);

	void writePlan(const Plan& p);

	void writePlan(const PolyPath& p);


	/** Return the number of states added to the file
	 * @return number of states
	 * */
	int size() const;

	/**
	 * Return number of lines added
	 * @return number of lines
	 */
	int lines() const;

	bool isLatLon() const;


	static void write(const std::string& filename, const std::vector<GeneralPlan> plist, const std::vector<double> activeTimes);

	static void write(const std::string& filename, const std::vector<GeneralPlan> plist);

	// ErrorReporter Interface Methods

	bool hasError() const;

	bool hasMessage() const;

	std::string getMessage();

	std::string getMessageNoClear() const;

	std::string toString() const;
};

} /* namespace larcfm */

#endif /* SRC_GENERALPLANWRITER_H_ */
