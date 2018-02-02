/*
 * Copyright (c) 2017 United States Government as represented by
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
    /** A new StateWriter. */
	GeneralStateWriter();

    /** A new StateReader based on the given file. */
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
