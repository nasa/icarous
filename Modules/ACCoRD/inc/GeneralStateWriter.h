/*
 * Copyright (c) 2017-2019 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */


#ifndef GENERALSTATEWRITER_H_
#define GENERALSTATEWRITER_H_

#include "GeneralState.h"
#include "ErrorLog.h"
#include "ErrorReporter.h"
#include "SeparatedOutput.h"
#include "Position.h"
#include "NavPoint.h"
#include "Velocity.h"
#include "SimplePoly.h"
#include "SimpleMovingPoly.h"
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
 * Required columns include aircraft name, 3 position columns (either x[nmi]/y[nmi]/z[ft] or 
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
class GeneralStateWriter : ErrorReporter {
private:
	ErrorLog error;
	SeparatedOutput output;
	bool velocity;
	bool latlon;
	bool trkgsvs;
	bool display_time;
	bool first_line;
	bool display_units;
	bool polygons; // includes polygons (needs extra header)
	int precision;
	int lines_i;
	int num;
	std::string fname;
	static const double default_time;
	std::ostream* fw;

public:
	/** A new GeneralStateWriter. */
	GeneralStateWriter();

	/** A new GeneralStateWriter based on the given file. */
	void open(const std::string& filename);

	void open(std::ostream* writer);

    void close();

//	bool isVelocity() const;
//
//	void setVelocity(bool velocity);

	bool isTrkGsVs() const;

	void setTrkGsVs(bool trkgsvs);

	int getPrecision() const;

	void setPrecision(int precision);

	/** Will the time be added to the file */
	bool isOutputTime() const;

	/** Should the time be added to the file */
	void setOutputTime(bool display_time);

	/** Will the units be displayed? */
	bool isOutputUnits() const;

	/** Should the units be displayed? */
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
	 */
	void addComment(const std::string& comment);

	/**
	 * Set parameters.  Use all the parameters in the reader.
	 */
	void setParameters(const ParameterData& pr);

	void writeState(const std::string& name, double time, const Position& p, const Velocity& v, const std::string& label);

	void writeState(const GeneralState& gs, const std::string& label);

	void writeState(const std::string& name, double time, const Position& p, const Velocity& v);

	void writeState(const std::string& name, double time, const Position& p, const std::string& label);

	void writeState(const std::string& name, double time, const Position& p);

	void writeState(const std::string& name, double time, std::pair<Position,Velocity> pv, const std::string& label);

	void writeState(const std::string& name, const Position& p, const Velocity& v, const std::string& label);

	void writeState(const std::string& name, const Position& p, const Velocity& v);

	void writeState(const std::string& name, const Position& p, const std::string& label);

	void writeState(const std::string& name, const NavPoint& np);

	void writeState(const std::string& name, double time, const SimpleMovingPoly& smp, const std::string& label, bool containment);

	void writeState(const std::string& name, double time, const SimplePoly& p, const std::vector<Velocity>& vlist, const std::string& label, bool containment);

	void writeState(const std::string& name, double time, const SimplePoly& p, const Velocity& v, const std::string& label, bool containment);

	void writeState(const std::string& name, double time, const SimpleMovingPoly& smp, bool containment);

	void writeState(const std::string& name, double time, const SimplePoly& p, const std::vector<Velocity>& vlist, bool containment);

	void writeState(const std::string& name, double time, const SimplePoly& p, const Velocity& v, bool containment);

	void writeState(const std::string& name, const SimpleMovingPoly& smp, bool containment);

	void writeState(const std::string& name, const SimplePoly& p, const std::vector<Velocity>& vlist, bool containment);

	void writeState(const std::string& name, const SimplePoly& p, const Velocity& v, bool containment);

	void writeState(const std::string& name, const SimpleMovingPoly& smp, const std::string& label, bool containment);

	void writeState(const std::string& name, const SimplePoly& p, const std::vector<Velocity>& vlist, const std::string& label, bool containment);

	void writeState(const std::string& name, const SimplePoly& p, const Velocity& v, const std::string& label, bool containment);

	void writeState(const GeneralState& gs);

	void writeState(const std::string& name, double time, const SimpleMovingPoly& smp, const std::string& label);

	void writeState(const std::string& name, double time, const SimplePoly& p, const std::vector<Velocity>& vlist, const std::string& label);

	void writeState(const std::string& name, double time, const SimplePoly& p, const Velocity& v, const std::string& label);

	void writeState(const std::string& name, double time, const SimpleMovingPoly& smp);

	void writeState(const std::string& name, double time, const SimplePoly& p, const std::vector<Velocity>& vlist);

	void writeState(const std::string& name, double time, const SimplePoly& p, const Velocity& v);

	void writeState(const std::string& name, const SimpleMovingPoly& smp);

	void writeState(const std::string& name, const SimplePoly& p, const std::vector<Velocity>& vlist);

	void writeState(const std::string& name, const SimplePoly& p, const Velocity& v);

	void writeState(const std::string& name, const SimpleMovingPoly& smp, const std::string& label);

	void writeState(const std::string& name, const SimplePoly& p, const std::vector<Velocity>& vlist, const std::string& label);

	void writeState(const std::string& name, const SimplePoly& p, const Velocity& v, const std::string& label);


	/** Return the number of states added to the file */
	int size() const;

	/**
	 * Return number of lines added
	 */
	int lines() const;

    bool isLatLon() const;

    std::string toString() const;


	// ErrorReporter Interface Methods

	bool hasError() const;
	bool hasMessage() const;
	std::string getMessage();
	std::string getMessageNoClear() const;

};

} /* namespace larcfm */

#endif /* SRC_GENERALSTATEWRITER_H_ */
