/*
 * Copyright (c) 2017 United States Government as represented by
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
	int lines_i;
	int num;
	std::string fname;
	PolyPath::PathMode mode;
	bool modedefined;
	std::ostream* fw;


public:
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
	 * @param m
	 */
	void setPolyPathMode(PolyPath::PathMode m);

	/**
	 * Clear any set polygon PathMode.  The PathMode will default to the first written item's path mode if it is a PolyPath or MORPHING if it is not.
	 * Whatever the stored PathMode, all polygons written will be converted to that type.
	 * @param m
	 */
	void clearPolyPathMode();

	/**
	 * If necessary, this must be called before the first write call
	 */
	void setParameterContainment(const std::vector<GeneralPlan>& list);

	void setParameterContainment(const std::string& s);

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


	// ErrorReporter Interface Methods

	bool hasError() const;

	bool hasMessage() const;

	std::string getMessage();

	std::string getMessageNoClear() const;

	std::string toString() const;
};

} /* namespace larcfm */

#endif /* SRC_GENERALPLANWRITER_H_ */
