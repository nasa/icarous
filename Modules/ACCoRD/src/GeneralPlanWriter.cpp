/*
 * Copyright (c) 2017-2019 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
/*
 * GeneralPlanWriter.cpp
 *
 *  Created on: Mar 8, 2017
 *      Author: ghagen
 */

#include "GeneralPlanWriter.h"
#include "GeneralPlan.h"
#include "ErrorLog.h"
#include "ErrorReporter.h"
#include "SeparatedOutput.h"
#include "Plan.h"
#include "PolyPath.h"
#include "ParameterData.h"
#include "format.h"
#include "string_util.h"
#include <vector>
#include <string>
#include <iostream>

namespace larcfm {

GeneralPlanWriter::GeneralPlanWriter() : error(ErrorLog("GeneralPlanWriter")), output(NULL) {
	trkgsvs = true;
	display_time = true;
	display_units = true;
	polygons = true;
	source = true;
	time2 = true;
	precision = 8;
    latLonExtraPrecision = 2;
	mode = PolyPath::MORPHING;
	modedefined = false;
	latlon = true;
	first_line = false;
	lines_i = 0;
	num = 0;

}

void GeneralPlanWriter::open(const std::string& filename) {
    std::ofstream out;
    out.open(filename.c_str());
    if ( out.fail() ) {
      error.addError("File "+filename+" write protected");
      return;
    }
    open(&out);
}

void GeneralPlanWriter::open(std::ostream* writer) {
	error = ErrorLog("GeneralPlaneWriter(Writer)");
	if (writer == NULL) {
		error.addError("Null supplied for Writer in open()");
		return;
	}
	fw = writer;
  output = SeparatedOutput(writer);
  first_line = true;
  lines_i = 0;
}

void GeneralPlanWriter::close() {
	if (fw != NULL) {
		output.close();
		try {
			std::ofstream* ffw = dynamic_cast<std::ofstream*>(fw);
			if (ffw != NULL) { // only do this if it is an ofstream
				ffw->close();
  			if (ffw->fail()) {
  				error.addError("Exception on close().");
  			}
			}
		} catch (std::exception& e) {
			std::cout << "GeneralPlanWriter::close: An exception occurred " << e.what() << '\n';
		}
		fw = NULL;
	}
}


bool GeneralPlanWriter::isTrkGsVs() const {
	return trkgsvs;
}

void GeneralPlanWriter::setTrkGsVs(bool trkgsvs_) {
	trkgsvs = trkgsvs_;
}

bool GeneralPlanWriter::hasPolygons() const {
	return polygons;
}

/**
 * Toggle extra polygon headers, true=include (default true)
 * @param p
 */
void GeneralPlanWriter::setPolygons(bool p) {
	polygons = p;
}


int GeneralPlanWriter::getPrecision() const {
	return precision;
}

void GeneralPlanWriter::setPrecision(int prec) {
	precision = prec;
}

int GeneralPlanWriter::getLatLonExtraPrecision() const {
	return latLonExtraPrecision;
}

void GeneralPlanWriter::setLatLonExtraPrecision(int prec) {
	latLonExtraPrecision = prec;
}

/** Will the time be added to the file
 * @return true, if the time is to be output to the file
 * */
bool GeneralPlanWriter::isOutputTime() const {
	return display_time;
}

/** Should the time be added to the file
 * @param display_time true, if the time is to be output to the file
 */
void GeneralPlanWriter::setOutputTime(bool display_time_) {
	display_time = display_time_;
}

/** Will the units be displayed?
 * @return true, if the units are to be displayed in output
 * */
bool GeneralPlanWriter::isOutputUnits() const {
	return display_units;
}

/** Should the units be displayed?
 * @param display_units true, if the units are to be displayed in output
 * */
void GeneralPlanWriter::setOutputUnits(bool display_units_) {
	display_units = display_units_;
}

/**
 * Sets the column delimiter to a tab.  This method can only be used before the first "writeState" method.
 */
void GeneralPlanWriter::setColumnDelimiterTab() {
	if (first_line) {
		output.setColumnDelimiterTab();
	}
}

/**
 * Sets the column delimiter to a comma.  This method can only be used before the first "writeState" method.
 */
void GeneralPlanWriter::setColumnDelimiterComma() {
	if (first_line) {
		output.setColumnDelimiterComma();
	}
}

/**
 * Sets the column delimiter to a space.  This method can only be used before the first "writeState" method.
 */
void GeneralPlanWriter::setColumnDelimiterSpace() {
	if (first_line) {
		output.setColumnDelimiterSpace();
		output.setEmptyValue("0");
	}
}

/**
 * Adds a comment line to the file.
 * @param comment comment string
 */
void GeneralPlanWriter::addComment(const std::string& comment) {
	output.addComment(comment);
}

/**
 * Set parameters.  Use all the parameters in the reader.
 * @param pr parameters
 */
void GeneralPlanWriter::setParameters(const ParameterData& pr) {
	output.setParameters(pr);
}

/**
 * Specify a polygon PathMode.  If this is not set ahead of time, this will default to the first written item's path mode if it is a PolyPath or MORPHING if it is not.
 * Whatever the stored PathMode, all polygons written will be converted to that type.
 * @param m
 */
void GeneralPlanWriter::setPolyPathMode(PolyPath::PathMode m) {
	mode = m;
	modedefined = true;
}

/**
 * Clear any set polygon PathMode.  The PathMode will default to the first written item's path mode if it is a PolyPath or MORPHING if it is not.
 * Whatever the stored PathMode, all polygons written will be converted to that type.
 * @param m
 */
void GeneralPlanWriter::clearPolyPathMode() {
	mode = PolyPath::MORPHING;
	modedefined = false;
}

/**
 * If necessary, this must be called before the first write call
 */
void GeneralPlanWriter::setPolyPathParameters(const std::vector<GeneralPlan>& list) {
	std::vector<GeneralPlan> pplist;
	if (list.size() > 0) {
		bool found = false;
		std::string s = "";
		for (int i = 0; i < (int) list.size(); i++) {
			if (list[i].hasPolyPath()) {
				if (!found) {
					setPolygons(true);
					setPolyPathMode(list[i].getPolyPath().getPathMode());
				}
				if (list[i].isContainment()) {
					pplist.push_back(list[i]);
				}
			}
		}
		if (pplist.size() > 0) {
			s = pplist[0].getID();
			for (int i = 1; i < (int) pplist.size(); i++) {
				s = s+","+pplist[i].getID();
			}
		}
		if (pplist.size() > 0) {
			setContainmentParameter(s);
		}
	}
}

void GeneralPlanWriter::setContainmentParameter(const std::string& s) {
	output.setParameter("containment", s);
}

void GeneralPlanWriter::writePlan(const GeneralPlan& gp, double activation_time) {
	if (first_line) {
		latlon = gp.isLatLon();
		output.setOutputUnits(display_units);
		if (gp.hasPolyPath()) {
			polygons = true;
			if (!modedefined) {
				mode = gp.getPolyPath().getPathMode();
				modedefined = true;
			}
		}

		if (modedefined) {
			output.setParameter("PathMode", PolyPath::pathModeToString(mode));
		}

		// Comments and parameters are handled by SeparatedOutput

		output.addHeading("name", "unitless");//0
		if (latlon) {
			output.addHeading("lat",  "deg");//1
			output.addHeading("lon",  "deg");//2
			output.addHeading("alt",  "ft");//3
		} else {
			output.addHeading("sx",   "NM");//1
			output.addHeading("sy",   "NM");//2
			output.addHeading("sz",   "ft");//3
		}
		if (display_time) {
			output.addHeading("time", "s");//4
		}
		output.addHeading("type","unitless");//5
		output.addHeading("tcp_trk","unitless");//9
		output.addHeading("tcp_gs","unitless");//11
		output.addHeading("tcp_vs","unitless");//13
		output.addHeading("radius", "NM");
		output.addHeading("accel_gs", "m/s^2");//12
		output.addHeading("accel_vs", "m/s^2");//14
		if (source) {
			if (latlon) {
				output.addHeading("src_lat",  "deg");//15
				output.addHeading("src_lon",  "deg");//16
				output.addHeading("src_alt",  "ft");//17
			} else {
				output.addHeading("src_sx",   "NM");//15
				output.addHeading("src_sy",   "NM");//16
				output.addHeading("src_sz",   "ft");//17
			}
			output.addHeading("src_time", "s");//18
		}
		if (latlon) {
			output.addHeading("center_lat", "deg");
			output.addHeading("center_lon", "deg");
			output.addHeading("center_alt",  "ft");
		} else {
			output.addHeading("center_x",   "NM");
			output.addHeading("center_y",   "NM");
			output.addHeading("center_z",   "ft");
		}
		output.addHeading("info", "unitless");//20
		output.addHeading("label", "unitless");//19
		if (polygons) {
			if (latlon) {
				output.addHeading("alt2", "ft");//21
			} else {
				output.addHeading("sz2", "ft");//21
			}
			if (trkgsvs) {
				output.addHeading("trk", "deg");//6
				output.addHeading("gs",  "knot");//7
				output.addHeading("vs",  "fpm");//8
			} else {
				output.addHeading("vx",  "knot");//6
				output.addHeading("vy",  "knot");//7
				output.addHeading("vz",  "fpm");//8
			}
		}
		if (time2) {
			output.addHeading("active_time", "s");//21
		}
		first_line = false;
	}

	if (gp.hasPolyPath()) {
		PolyPath pp = gp.getPolyPath();
		if (modedefined && mode != pp.getPathMode()) {
			pp.setPathMode(mode);
		}
		for (int i = 0; i < pp.size(); i++) {
			for (int j = 0; j < pp.getPolyRef(i).size(); j++) {
				output.addColumn(pp.toStringList(i, j, precision, latLonExtraPrecision, true));
				if (pp.getPathMode() == PolyPath::MORPHING || pp.getPathMode() == PolyPath::AVG_VEL) {
					output.addColumn("-"); // trk
					output.addColumn("-"); // gs
					output.addColumn("-"); // vs
				}
				output.addColumn(Fm6(activation_time));
				lines_i++;
				output.writeLine();
			}
		}
	} else {
		Plan p = gp.getPlan();
		for (int i = 0; i < p.size(); i++) {
			output.addColumn(p.toStringList(i, precision, true));
			output.addColumn("-"); // alt2
			output.addColumn("-"); // trk
			output.addColumn("-"); // gs
			output.addColumn("-"); // vs
			output.addColumn(Fm6(activation_time));
			lines_i++;
			output.writeLine();
		}
	}
	num++;
}

void GeneralPlanWriter::writePlan(const GeneralPlan& gp) {
	writePlan(gp,0.0);
}

void GeneralPlanWriter::writePlan(const Plan& p, double t) {
	writePlan(GeneralPlan(p), t);
}

void GeneralPlanWriter::writePlan(const PolyPath& p, double t) {
	writePlan(GeneralPlan(p), t);
}

void GeneralPlanWriter::writePlan(const Plan& p) {
	writePlan(GeneralPlan(p));
}

void GeneralPlanWriter::writePlan(const PolyPath& p) {
	writePlan(GeneralPlan(p));
}


/** Return the number of states added to the file
 * @return number of states
 * */
int GeneralPlanWriter::size() const {
	return num;
}

/**
 * Return number of lines added
 * @return number of lines
 */
int GeneralPlanWriter::lines() const {
	return lines_i;
}

bool GeneralPlanWriter::isLatLon() const {
	return latlon;
}


void GeneralPlanWriter::write(const std::string& filename, const std::vector<GeneralPlan> plist, const std::vector<double> activeTimes) {
	GeneralPlanWriter pw;
	if (activeTimes.size() != 0 && activeTimes.size() != plist.size()) {
		pw.error.addError("write arguments are not the same size");
	} else {
		pw.open(filename);
		if (pw.hasError()) {
			pw.error.addError("write error: output is null");
		} else if (plist.size() > 0) {
			pw.latlon = plist[0].isLatLon();
			for (int i = 0; i < (int) plist.size(); i++) {
				GeneralPlan gp = plist[i];
				std::string note = "";
				if (gp.hasPlan()) {
					note = gp.getPlan().getNote();
				} else {
					pw.polygons = true;
					pw.mode = gp.getPolyPath().getPathMode();
					note = gp.getPolyPath().getNote();
				}
				if (!equals(note,"")) {
					pw.output.setParameter(plist[i].getID()+"_note", note);
				}
			}
			if (activeTimes.size() == plist.size()) {
				for (int i = 0; i < (int) plist.size(); i++) {
					pw.writePlan(plist[i], activeTimes[i]);
				}
			} else {
				for (int i = 0; i < (int) plist.size(); i++) {
					pw.writePlan(plist[i]);
				}
			}
		}
	}
	if (pw.hasError()) {
		std::cout << "GeneralPlanWriter.write error: " << pw.getMessage() << std::endl;
	}
}

void GeneralPlanWriter::write(const std::string& filename, const std::vector<GeneralPlan> plist) {
	std::vector<double> ts;
	write(filename,plist,ts);
}


// ErrorReporter Interface Methods

bool GeneralPlanWriter::hasError() const {
	return error.hasError() || output.hasError();
}
bool GeneralPlanWriter::hasMessage() const {
	return error.hasMessage() || output.hasMessage();
}
std::string GeneralPlanWriter::getMessage() {
	return error.getMessage() + output.getMessage();
}
std::string GeneralPlanWriter::getMessageNoClear() const {
	return error.getMessageNoClear() + output.getMessageNoClear();
}


} /* namespace larcfm */
