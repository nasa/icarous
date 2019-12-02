/*
 * Copyright (c) 2016-2018 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
#ifndef GENERALSTATEREADER_H
#define GENERALSTATEREADER_H

#include "ErrorLog.h"
#include "ErrorReporter.h"
#include "SeparatedInput.h"
#include "GeneralState.h"
#include "ParameterProvider.h"
#include "ParameterReader.h"
#include "ErrorReporter.h"
#include <string>
#include <vector>

namespace larcfm {

/**
 * This reads in and stores a set of aircraft and polygon states, possibly over time, (and
 * parameters) from a file The aircraft states are stored in an
 * ArrayList&lt;GeneralState&gt;. A state file consists of comma or
 * space-separated values, with one position and velocity per line. Required
 * columns include aircraft name, 3 position columns (either x[NM]/y[NM]/z[ft]
 * or latitude[deg]/longitude[deg]/altitude[ft]) and 3 velocity columns (either
 * vx[kn]/vy[kn]/vz[fpm] or track[deg]/gs[kn]/vs[fpm]).
 * All empty lines or comment lines (starting with a hash sign (#)) are ignored.
 * Polygon information is one vertex per line, with an additional "top" column to
 * denote the polygon's upper altitude.
 * <p>
 *
 * While this reader allows for a time column, only the last
 * entry for an aircraft or polygon will be stored.  The GeneralSequenceReader is 
 * needed to allow for multiple state-time data points. 
 * If consecutive position and velocity lines are for the
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
 *
 */
class GeneralStateReader : public ParameterProvider, public ParameterReader, public ErrorReporter {
protected:
	mutable ErrorLog error;
	SeparatedInput input;
	std::vector<GeneralState> states;
	bool hasRead;
	bool latlon;
	bool trkgsvs;
	bool clock;

	// we store the heading indices in the following order:
	enum {
		NAME,
		LAT_SX,
		LON_SY,
		ALT_SZ,
		TRK_VX,
		GS_VY,
		VS_VZ,
		TM_CLK,
		SZ2
	};

	static const int head_length = SZ2 + 1;
	int head[head_length];

	enum {
		POLY,
		UNKNOWN,
		STATE
	};

	std::string fname;


public:
	/** A new, empty StateReader. After you have a StateReader object then use the open() method. */
	GeneralStateReader();

	/** Read a new file into an existing StateReader.  Parameters are preserved if they are not specified in the file. */
	void open(const std::string& filename);

	/** Read a new file into an existing StateReader.  Parameters are preserved if they are not specified in the file. */
	void open(std::istream* ins);

private:
	void close();

	void loadfile();

protected:
	int altHeadings(const std::string& s1, const std::string& s2, const std::string& s3, const std::string& s4) const;

	int altHeadings(const std::string& s1, const std::string& s2, const std::string& s3) const;

	int altHeadings(const std::string& s1, const std::string& s2) const;

	/**
	 * Return the string as a single value in seconds.   If the column is labeled "clock," then
	 * it is expected in a "HH:MM:SS" format.  If the column is labeled "time" then just read
	 * it as a value.  If the string cannot be parsed, return 0.0;
	 * @param s the string to be parsed
	 * @return
	 */
	double parseClockTime(const std::string& s) const;

	int getIndex(const std::string& name) const;

	int altHeadings5(const std::string& s1, const std::string& s2, const std::string& s3, const std::string& s4, const std::string& s5) const;

public:
	/** Return the number of AircraftStates in the file */
	int size() const;

	int numAircraft() const;

	int numPolygons() const;

	/** Returns the i-th aircraft state in the file.  This will be a deep copy. */
	GeneralState getGeneralState(int ac) const;

	std::vector<GeneralState> getGeneralStateList() const;

	bool isLatLon() const;

	std::string getFilename() const;


	ParameterData getParameters() const {
		return input.getParameters();
	}

	ParameterData& getParametersRef() {
		return input.getParametersRef();
	}

	void updateParameterData(ParameterData& p) const {
		p.copy(input.getParameters(), true);
	}


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

	std::string toString() const;
};

}

#endif //PLANREADER_H
