/* 
 * StateWriter
 *
 * Contact: Jeff Maddalon
 * 
 * Copyright (c) 2011-2019 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */


#ifndef PLANWRITER_H
#define PLANWRITER_H

#include "ErrorLog.h"
#include "ErrorReporter.h"
#include "SeparatedOutput.h"
#include "ParameterData.h"
#include "Plan.h"
#include "PolyPath.h"
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <stdexcept>

namespace larcfm {

class PlanWriter : public ErrorReporter {
private:
	ErrorLog error;
	SeparatedOutput output;
	bool latlon;
	bool first_line;
	bool display_units;
	bool tcpColumns;
	bool polyColumns;
	bool trkgsvs;
	int precision;
	int latLonExtraPrecision;
	int lines;
	std::string fname;
	std::ofstream fw;
	PolyPath::PathMode mode;

//	double timeOffset;

	void writeHeader(bool write_tcp, bool ll);

public:
    /** A new PlanWriter. */
	PlanWriter();
	/** A new StateReader based on the given file. 
	 * @param filename filename
	 */
	void open(const std::string& filename);

	void open(std::ostream* writer);

    void close();
	
	int getPrecision() const;

	void setPrecision(int precision);

	int getLatLonExtraPrecision() const;

	void setLatLonExtraPrecision(int precision);

	double getTimeOffset() const;

	void setTimeOffset(double offset);

	/**
	 * Indicate polygons will be output and set the path mode to be output.  This must be called before the first write command,
	 * otherwise any attempts to write a polygon will result in a warning and no polygon written.
	 * The default mode is PolyPath.PathMode.AVG_VEL.
	 * @param m mode to write polygons.
	 */
	void setPolyPathMode(PolyPath::PathMode m);

	void setPolygons(bool b);

	/**
	 * Indicate that no polygons will be output (the default).
	 */
	void clearPolyPathMode();

	/**
	 * Indicate which polygons are considered containment (geofences).  This must be called before the first write command, which ignores stored containment information.
	 */
	void setContainmentParameter(const std::string& s);

	void setPolyPathParameters(const std::vector<PolyPath>& list);

	/**
	 * Sets parameters for the listed plans.  Currently this consists of the "note" field for each plan, if present.
	 * @param plans
	 */
	void setPlanParameters(const std::vector<Plan>& plans);

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
	 * Additively set parameters.  (This does not delete existing parameters, but will overwrite them.)
	 * @param pr parameters
	 */
	void setParameters(const ParameterData& pr);
	
	void clearParameters();

//	void writeLn(const std::string& str);

	void writePlan(const Plan& p, bool tcpColumnsLocal);

	void writePolyPath(const PolyPath& p, bool tcpColumnsLocal);
	
	/** Return the number of lines added to the file 
	 * @return size
	 */
	int size() const;

    bool isLatLon() const;
    
    void flush();
	
	// ErrorReporter Interface Methods

	bool hasError() const;
	bool hasMessage() const;
	std::string getMessage();
	std::string getMessageNoClear() const;

	static void write(const std::string& filename, const std::vector<Plan>& planlist, const std::vector<PolyPath>& polylist, bool tcpColumns);

};
}

#endif //PLANWRITER_H
