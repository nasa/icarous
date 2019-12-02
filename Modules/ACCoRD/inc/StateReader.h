/* 
 * StateReader
 *
 * Contact: George Hagen
 *
 * Copyright (c) 2011-2018 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#ifndef STATEREADER_H
#define STATEREADER_H

#include "ErrorLog.h"
#include "ErrorReporter.h"
#include "SeparatedInput.h"
#include "AircraftState.h"
#include "Position.h"
#include "Velocity.h"
#include "ParameterReader.h"
#include "ParameterProvider.h"
#include "Triple.h"
#include <string>
#include <map>
#include <vector>
#include <iostream>

namespace larcfm {

/**
 * This reads in and stores a set of aircraft states, possibly over time, (and
 * parameters) from a file The aircraft states are stored in an
 * ArrayList&lt;AircraftState&gt;. A state file consists of comma or
 * space-separated values, with one position and velocity per line. Required
 * columns include aircraft name, 3 position columns (either x[NM]/y[NM]/z[ft]
 * or latitude[deg]/longitude[deg]/altitude[ft]) and 3 velocity columns (either
 * vx[kn]/vy[kn]/vz[fpm] or track[deg]/gs[kn]/vs[fpm]).
 * All empty lines or comment lines (starting with a hash sign (#)) are ignored.
 * <p>
 *
 * An optional column is time [s]. If it is included, a "history" will be build
 * if an aircraft has more than one entry. If it is not included, only the last
 * entry for an aircraft will be stored. If multiple aircraft and time are
 * included (a fairly conventional case) then all the table must be organized
 * with all the data for one aircraft listed consecutively. Thus, all the data
 * for the first aircraft must be grouped together, then all the data for the
 * second aircraft, etc. If consecutive position and velocity lines are for the
 * same aircraft, subsequent name fields may be replaced with a double quotation
 * mark (&quot;). The aircraft name is case sensitive, so US54A != Us54a !=
 * us54a.
 * <p>
 *
 * It is necessary to include a header line that defines the column ordering.
 * The column definitions are not case sensitive. There is also an optional
 * header line, immediately following the column definition, that defines the
 * unit type for each column (the defaults are listed above).
 * <p>
 *
 * Files may also include parameter definitions prior to other data. Parameter
 * definitions are of the form &lt;key&gt; = &lt;value&gt;, one per line, where
 * &lt;key&gt; is a case-insensitive alphanumeric word and &lt;value&gt; is
 * either a numeral or string. The &lt;value&gt; may include a unit, such as
 * "dist = 50 [m]". Note that parameters require a space on either side of the
 * equals sign. Note that it is possible to also update the stored parameter
 * values (or store additional ones) through API calls.
 * Parameters can be interpreted as double values, strings, or Boolean values,
 * and the user is required to know which parameter is interpreted as which
 * type.  However, something reasonable will come out, for instance a double 
 * value read as a string will come out as the string representation of the value.
 * <p>
 *
 * If the optional parameter "filetype" is specified, its value must be "state"
 * or "history" (no quotes) for this reader to accept the file without error.
 * <p>
 *
 * This allows for arbitrary additional user-defined columns.  
 * New columns' information may be accessed by the get getNewColumnValue(), 
 * getNewColumnBoolean(), or getNewColumnString() methods.
 * If there are multiple time-points in the file, only the values for the last 
 * time for an aircraft will be stored.
 * New columns are assumed unitless unless units are specified.
 *
 */
class StateReader: public ErrorReporter, public ParameterReader, public ParameterProvider {
private:
	void loadfile();

protected:
	// we store the heading indices in the following order:
	enum {
		NAME, LAT_SX, LON_SY, ALT_SZ, TRK_VX, GS_VY, VS_VZ, TM_CLK
	};
	static const int definedColumns = TM_CLK + 1;

	typedef std::map<std::pair<int,int>, Triple<double,bool, std::string> > extraTblType;
	extraTblType extracolumnValues;

	mutable ErrorLog error;
	SeparatedInput input;
	std::vector<AircraftState> states;
	bool hasRead;
	bool latlon;
	bool trkgsvs;
	bool clock;
	std::vector<int> head;

	bool interpretUnits;

	int altHeadings(const std::string& s1, const std::string& s2,
			const std::string& s3, const std::string& s4) const;
	int altHeadings(const std::string& s1, const std::string& s2,
			const std::string& s3) const;
	int altHeadings(const std::string& s1, const std::string& s2) const;
	/**
	 * Return the string as a single value in seconds.   If the column is labeled "clock," then
	 * it is expected in a "HH:MM:SS" format.  If the column is labeled "time" then just read
	 * it as a value.  If the string cannot be parsed, return 0.0;
	 * @param s the string to be parsed
	 * @return
	 */
	double parseClockTime(const std::string& s) const;
	int getIndex(const std::string& s) const;

public:

    /** A new, empty StateReader. After you have a StateReader object then use the open() method. */
	StateReader();

	/** Read a new file into an existing StateReader.  Parameters are preserved if they are not specified in the file. 
	 * @param filename file name
	 */
	virtual void open(const std::string& filename);

	/** Read a new stream into an existing StateReader.  Parameters are preserved if they are not specified in the file. */
	virtual void open(std::istream* ins);

	ParameterData& getParametersRef();
	ParameterData getParameters() const;
	void updateParameterData(ParameterData& p) const;

  /** Return the number of AircraftStates in the file 
   * @return size 
   * */
	int size() const;
	/** Returns the i-th AircraftState in the file */
	AircraftState getAircraftState(int i) const;

	/** Returns the list of all AircraftStates in the file */
	std::vector<AircraftState> getAircraftStateList() const;

	/** Returns the (most recent) position of the i-th aircraft state in the file.  This is the raw position, and has not been through any projection. 
	 * @param ac aircraft index
	 * @return position
	 * */
	Position getPosition(int ac) const;

	/** Returns the (most recent) velocity of the i-th aircraft state in the file.   This is the raw velocity, and has not been through any projection. 
	 * @param ac aircraft index
	 * @return velocity
	 * */
	Velocity getVelocity(int ac) const;

	/** returns the string name of aircraft i 
	 * @param ac aircraft index
	 * @return name 
	 */
	std::string getName(int ac) const;

	double getTime(int ac) const;

	bool isLatLon() const;


	/**
	 * Return a list of all user-defined columns in this reader.
	 */
	std::vector<std::string> getExtraColumnList() const;

	/**
	 * Return the units for a given column.  If no units were specified, then return "unspecified".  If the column name was not found, return the empty string.
	 * @param colname name of the column in question.
	 */
	std::string getExtraColumnUnits(const std::string& colname) const;

	/**
	 * Return true if the given aircraft has data for the indicated column.
	 * @param ac
	 * @param colname
	 * @return
	 */
	bool hasExtraColumnData(int ac, const std::string& colname) const;

	/**
	 * Returns the column value associated with a given aircraft, interpreted as a double, or NaN if there is no info
	 */
	double getExtraColumnValue(int ac, const std::string& colname) const;

	/**
	 * Returns the column value associated with a given aircraft, interpreted as a boolean, or false if there is no info
	 */
	bool getExtraColumnBool(int ac, const std::string& colname) const;

	/**
	 * Returns the column value associated with a given aircraft, interpreted as a string, or the empty string if there is no info
	 */
	std::string getExtraColumnString(int ac, const std::string& colname) const;

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

#endif //STATEREADER_H
