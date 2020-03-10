/*
 * Copyright (c) 2017-2019 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
/*
 * GeneralGeneralStateWriter.cpp
 *
 *  Created on: Mar 7, 2017
 *      Author: ghagen
 */

#include "GeneralStateWriter.h"
#include "GeneralState.h"
#include "Position.h"
#include "Velocity.h"
#include "NavPoint.h"
#include "SimplePoly.h"
#include "SimpleMovingPoly.h"
#include "string_util.h"
#include "format.h"
#include <cstdio>
#include <iostream>
#include <sstream>
#include <vector>
#include <string>

namespace larcfm {


using std::string;
using std::vector;

const double GeneralStateWriter::default_time = 0.0;

	GeneralStateWriter::GeneralStateWriter() : error("GeneralStateWriter"), output(NULL) {
		//error = new ErrorLog("GeneralStateWriter");
      trkgsvs = true;
      velocity = true;
      display_time = true;
      display_units = true;
      precision = 6;
      first_line = false;
      latlon = false;
      lines_i= 0;
      fw = NULL;
	}

	void GeneralStateWriter::open(const std::string& filename) {
	    std::ofstream out;
	    out.open(filename.c_str());
	    if ( out.fail() ) {
	      error.addError("File "+filename+" write protected");
	      return;
	    }
	    open(&out);
	}

	void GeneralStateWriter::open(std::ostream* writer) {
		error = ErrorLog("GeneralStateWriter(Writer)");
		if (writer == NULL) {
			error.addError("Null supplied for Writer in open()");
			return;
		}
		fw = writer;
      output = SeparatedOutput(writer);
      first_line = true;
      lines_i = 0;
	}

  void GeneralStateWriter::close() {
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
			std::cout << "GeneralStateWriter::close: An exception occurred " << e.what() << '\n';
  		}
  		fw = NULL;
  	}
  }

//	bool GeneralStateWriter::isVelocity() const {
//		return velocity;
//	}
//
//	void GeneralStateWriter::setVelocity(bool vel) {
//		velocity = vel;
//	}

	bool GeneralStateWriter::isTrkGsVs() const {
		return trkgsvs;
	}

	void GeneralStateWriter::setTrkGsVs(bool trk_gs_vs) {
		trkgsvs = trk_gs_vs;
	}

	int GeneralStateWriter::getPrecision() const {
		return precision;
	}

	void GeneralStateWriter::setPrecision(int precision_digits) {
		precision = precision_digits;
	}

	bool GeneralStateWriter::isOutputTime() const {
		return display_time;
	}

	void GeneralStateWriter::setOutputTime(bool displaytime) {
		display_time = displaytime;
	}

	/** Will the units be displayed? */
	bool GeneralStateWriter::isOutputUnits() const {
		return display_units;
	}

	/** Should the units be displayed? */
	void GeneralStateWriter::setOutputUnits(bool displayunits) {
		display_units = displayunits;
	}

	void GeneralStateWriter::setColumnDelimiterTab() {
		if (first_line) {
			output.setColumnDelimiterTab();
		}
	}

	void GeneralStateWriter::setColumnDelimiterComma() {
		if (first_line) {
			output.setColumnDelimiterComma();
		}
	}

	void GeneralStateWriter::setColumnDelimiterSpace() {
		if (first_line) {
			output.setColumnDelimiterSpace();
			output.setEmptyValue("0");
		}
	}

	void GeneralStateWriter::addComment(const std::string& comment) {
		output.addComment(comment);
	}

	void GeneralStateWriter::setParameters(const ParameterData& pr) {
		output.setParameters(pr);
	}

	void GeneralStateWriter::writeState(const std::string& name, double time, const Position& p, const Velocity& v, const std::string& label) {
		writeState(GeneralState(name, p, v, time), label);
	}

	void GeneralStateWriter::writeState(const GeneralState& gs, const std::string& label) {
		std::string name = gs.getName();
		double time = gs.getTime();
		Velocity v = gs.getVelocity();
		if (first_line) {
			latlon = gs.isLatLon();
			output.setOutputUnits(display_units);
			if (gs.hasPolygon()) {
				polygons = true;
			}

			// Comments and parameters are handled by SeparatedOutput

			output.addHeading("name", "unitless");
			if (display_time) {
				output.addHeading("time", "s");
			}
			if (latlon) {
				output.addHeading("lat",  "deg");
				output.addHeading("lon",  "deg");
				output.addHeading("alt",  "ft");
			} else {
				output.addHeading("sx",   "NM");
				output.addHeading("sy",   "NM");
				output.addHeading("sz",   "ft");
			}
			if (polygons) {
				output.addHeading("alt2", "ft");
			}
			if (velocity) {
				if (trkgsvs) {
					output.addHeading("trk", "deg");
					output.addHeading("gs",  "knot");
					output.addHeading("vs",  "fpm");
				} else {
					output.addHeading("vx",  "knot");
					output.addHeading("vy",  "knot");
					output.addHeading("vz",  "fpm");
				}
			}
			output.addHeading("label", "");
			first_line = false;
		}

		if (gs.hasPolygon()) {
			for (int i = 0; i < gs.getPolygon().size(); i++) {
				output.addColumn(name);
				if (display_time) {
					output.addColumn(FmPrecision(time,precision));
				}
				output.addColumn(gs.getPolygon().toStringList(i, trkgsvs, precision));
				if (!equals(label,"")) output.addColumn(label);
				lines_i++;
				output.writeLine();
			}
		} else {
			output.addColumn(name);
			if (display_time) {
				output.addColumn(FmPrecision(time,precision));
			}
			output.addColumn(gs.getPosition().toStringList(precision));
			if (polygons) output.addColumn("-"); // add blank alt2 column
			if (velocity) {
				if (trkgsvs) {
					output.addColumn(v.toStringList(precision));
				} else {
					output.addColumn(v.toStringXYZList(precision));
				}
			}
			if (equals(label,"")) output.addColumn(label);
			lines_i++;
			output.writeLine();
		}
		num++;
	}

	void GeneralStateWriter::writeState(const std::string& name, double time, const Position& p, const Velocity& v) {
		writeState(name,time,p,v,"");
	}

	void GeneralStateWriter::writeState(const std::string& name, double time, const Position& p, const std::string& label) {
		writeState(name,time,p,Velocity::ZEROV(), label);
	}

	void GeneralStateWriter::writeState(const std::string& name, double time, const Position& p) {
		writeState(name,time,p,Velocity::ZEROV(), "");
	}

	void GeneralStateWriter::writeState(const std::string& name, double time, std::pair<Position,Velocity> pv, const std::string& label) {
		writeState(name, time, pv.first, pv.second, label);
	}

	void GeneralStateWriter::writeState(const std::string& name, const Position& p, const Velocity& v, const std::string& label) {
		writeState(name, default_time, p, v, label);
	}

	void GeneralStateWriter::writeState(const std::string& name, const Position& p, const Velocity& v) {
		writeState(name, default_time, p, v, "");
	}

	void GeneralStateWriter::writeState(const std::string& name, const Position& p, const std::string& label) {
		writeState(name, default_time, p, label);
	}

	void GeneralStateWriter::writeState(const std::string& name, const NavPoint& np) {
		writeState(name,np.time(),np.position(),Velocity::ZEROV(),np.name());
	}

	void GeneralStateWriter::writeState(const std::string& name, double time, const SimpleMovingPoly& smp, const std::string& label, bool containment) {
		writeState(GeneralState(name, smp, time, containment), label);
	}

	void GeneralStateWriter::writeState(const std::string& name, double time, const SimplePoly& p, const std::vector<Velocity>& vlist, const std::string& label, bool containment) {
		writeState(name, time, SimpleMovingPoly(p,vlist), label, containment);
	}

	void GeneralStateWriter::writeState(const std::string& name, double time, const SimplePoly& p, const Velocity& v, const std::string& label, bool containment) {
		writeState(name, time, SimpleMovingPoly(p,v), label, containment);
	}

	void GeneralStateWriter::writeState(const std::string& name, double time, const SimpleMovingPoly& smp, bool containment) {
		writeState(name, time, smp,  "", containment);
	}

	void GeneralStateWriter::writeState(const std::string& name, double time, const SimplePoly& p, const std::vector<Velocity>& vlist, bool containment) {
		writeState(name, time, SimpleMovingPoly(p,vlist), "", containment);
	}

	void GeneralStateWriter::writeState(const std::string& name, double time, const SimplePoly& p, const Velocity& v, bool containment) {
		writeState(name, time, SimpleMovingPoly(p,v), "", containment);
	}

	void GeneralStateWriter::writeState(const std::string& name, const SimpleMovingPoly& smp, bool containment) {
		writeState(name, default_time, smp,  "", containment);
	}

	void GeneralStateWriter::writeState(const std::string& name, const SimplePoly& p, const std::vector<Velocity>& vlist, bool containment) {
		writeState(name, default_time, SimpleMovingPoly(p,vlist), "", containment);
	}

	void GeneralStateWriter::writeState(const std::string& name, const SimplePoly& p, const Velocity& v, bool containment) {
		writeState(name, default_time, SimpleMovingPoly(p,v), "", containment);
	}

	void GeneralStateWriter::writeState(const std::string& name, const SimpleMovingPoly& smp, const std::string& label, bool containment) {
		writeState(name, default_time, smp, label, containment);
	}

	void GeneralStateWriter::writeState(const std::string& name, const SimplePoly& p, const std::vector<Velocity>& vlist, const std::string& label, bool containment) {
		writeState(name, default_time, SimpleMovingPoly(p,vlist), label, containment);
	}

	void GeneralStateWriter::writeState(const std::string& name, const SimplePoly& p, const Velocity& v, const std::string& label, bool containment) {
		writeState(name, default_time, SimpleMovingPoly(p,v), label, containment);
	}

	void GeneralStateWriter::writeState(const GeneralState& gs) {
		writeState(gs,"");
	}

	void GeneralStateWriter::writeState(const std::string& name, double time, const SimpleMovingPoly& smp, const std::string& label) {
		writeState(GeneralState(name, smp, time, false), label);
	}

	void GeneralStateWriter::writeState(const std::string& name, double time, const SimplePoly& p, const std::vector<Velocity>& vlist, const std::string& label) {
		writeState(name, time, SimpleMovingPoly(p,vlist), label, false);
	}

	void GeneralStateWriter::writeState(const std::string& name, double time, const SimplePoly& p, const Velocity& v, const std::string& label) {
		writeState(name, time, SimpleMovingPoly(p,v), label, false);
	}

	void GeneralStateWriter::writeState(const std::string& name, double time, const SimpleMovingPoly& smp) {
		writeState(name, time, smp,  "", false);
	}

	void GeneralStateWriter::writeState(const std::string& name, double time, const SimplePoly& p, const std::vector<Velocity>& vlist) {
		writeState(name, time, SimpleMovingPoly(p,vlist), "", false);
	}

	void GeneralStateWriter::writeState(const std::string& name, double time, const SimplePoly& p, const Velocity& v) {
		writeState(name, time, SimpleMovingPoly(p,v), "", false);
	}

	void GeneralStateWriter::writeState(const std::string& name, const SimpleMovingPoly& smp) {
		writeState(name, default_time, smp,  "", false);
	}

	void GeneralStateWriter::writeState(const std::string& name, const SimplePoly& p, const std::vector<Velocity>& vlist) {
		writeState(name, default_time, SimpleMovingPoly(p,vlist), "", false);
	}

	void GeneralStateWriter::writeState(const std::string& name, const SimplePoly& p, const Velocity& v) {
		writeState(name, default_time, SimpleMovingPoly(p,v), "", false);
	}

	void GeneralStateWriter::writeState(const std::string& name, const SimpleMovingPoly& smp, const std::string& label) {
		writeState(name, default_time, smp, label, false);
	}

	void GeneralStateWriter::writeState(const std::string& name, const SimplePoly& p, const std::vector<Velocity>& vlist, const std::string& label) {
		writeState(name, default_time, SimpleMovingPoly(p,vlist), label, false);
	}

	void GeneralStateWriter::writeState(const std::string& name, const SimplePoly& p, const Velocity& v, const std::string& label) {
		writeState(name, default_time, SimpleMovingPoly(p,v), label, false);
	}


	/** Return the number of states added to the file */
	int GeneralStateWriter::size() const {
		return num;
	}

	int GeneralStateWriter::lines() const {
		return lines_i;
	}

  bool GeneralStateWriter::isLatLon() const {
  	return latlon;
  }

  std::string GeneralStateWriter::toString() const {
  	std::string rtn = "GeneralStateWriter: ------------------------------------------------\n";
//    	for (int j = 0; j < states.size(); j++) {
//            rtn = rtn + states.get(j)+ "\n";
//    	}
  	return rtn;
  }

	// ErrorReporter Interface Methods

	bool GeneralStateWriter::hasError() const {
		return error.hasError() || output.hasError();
	}
	bool GeneralStateWriter::hasMessage() const {
		return error.hasMessage() || output.hasMessage();
	}
	std::string GeneralStateWriter::getMessage() {
		return error.getMessage() + output.getMessage();
	}
	std::string GeneralStateWriter::getMessageNoClear() const {
		return error.getMessageNoClear() + output.getMessageNoClear();
	}



} /* namespace larcfm */
