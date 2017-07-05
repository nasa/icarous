/*
 * Copyright (c) 2016-2017 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
#include "GeneralStateReader.h"
#include "Units.h"
#include "ErrorLog.h"
#include "ErrorReporter.h"
#include "SeparatedInput.h"
#include "GeneralState.h"
#include "string_util.h"
#include "format.h"
#include "Constants.h"
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <set>
#include <stdexcept>

namespace larcfm {

using std::string;
using std::vector;

GeneralStateReader::GeneralStateReader(): error("GeneralStateReader()") {
  input.setCaseSensitive(false);            // headers & parameters are lower case
  hasRead = false;
  latlon = false;
  clock = false;
  trkgsvs = false;
  for (int i = 0; i < head_length; i++) {
  	head[i] = 0;
  }
}


/** Read a new file into an existing StateReader.  Parameters are preserved if they are not specified in the file. */
void GeneralStateReader::open(const std::string& filename) {
    std::ifstream in;
    in.open(filename.c_str());
    if ( in.fail() ) {
      error.addError("File "+filename+" read protected or not found");
      states.clear();
      return;
    }
    error = ErrorLog("GeneralStateReader("+filename+")");
    open(&in);
    in.close();
}

/** Read a new file into an existing StateReader.  Parameters are preserved if they are not specified in the file. */
void GeneralStateReader::open(std::istream* in) {
    SeparatedInput si(in);
    si.setCaseSensitive(false);            // headers & parameters are lower case
    vector<string> params = input.getParametersRef().getList();
    for (unsigned int i = 0; i < params.size(); i++) {
      si.getParametersRef().set(params[i], input.getParametersRef().getString(params[i]));
    }
    input = si;
    loadfile();
}


void GeneralStateReader::loadfile() {
	hasRead = false;
	clock = true;
	string name = ""; // the current aircraft name
	double lastTime = -1000000; // time must be increasing
	int stateIndex = -1;
	std::set<string> containmentList;

	// save accuracy info in temp vars
	double h = Constants::get_horizontal_accuracy();
	double v = Constants::get_vertical_accuracy();
	double t = Constants::get_time_accuracy();


	while ( ! input.readLine()) {
		// look for each possible heading
		if ( ! hasRead) {

			// process heading
			latlon = (altHeadings("lat", "lon", "long", "latitude") >= 0);
			clock = (altHeadings("clock", "") >= 0);
			trkgsvs = (altHeadings("trk", "track") >= 0);

			head[NAME]   = altHeadings("name", "aircraft", "id");
			head[LAT_SX] = altHeadings("sx", "lat", "latitude");
			head[LON_SY] = altHeadings("sy", "lon", "long", "longitude");
			head[ALT_SZ] = altHeadings5("sz", "alt", "altitude", "sz1", "alt1");
			head[SZ2]    = altHeadings("sz2", "alt2", "altitude_top", "poly_top");
			head[TRK_VX] = altHeadings("trk", "vx", "track");
			head[GS_VY]  = altHeadings("gs", "vy", "groundspeed", "groundspd");
			head[VS_VZ]  = altHeadings("vs", "vz", "verticalspeed", "hdot");
			head[TM_CLK] = altHeadings("clock", "time", "tm", "st");

			// set accuracy parameters
			if (getParametersRef().contains("horizontalAccuracy")) {
				Constants::set_horizontal_accuracy(getParametersRef().getValue("horizontalAccuracy","m"));
			}
			if (getParametersRef().contains("verticalAccuracy")) {
				Constants::set_vertical_accuracy(getParametersRef().getValue("verticalAccuracy","m"));
			}
			if (getParametersRef().contains("timeAccuracy")) {
				Constants::set_time_accuracy(getParametersRef().getValue("timeAccuracy","s"));
			}

			if (getParametersRef().contains("filetype")) {
				string sval = getParametersRef().getString("filetype");
				if (!equalsIgnoreCase(sval,"state") && !equalsIgnoreCase(sval,"generalstate")) {
					error.addError("Wrong filetype: "+sval);
					break;
				}
			}

			if (getParametersRef().contains("containment")) {
				vector<string> vcl = getParametersRef().getListString("containment");
				for (int i = 0; i < (int) vcl.size(); i++) {
					string s = vcl[i];
					containmentList.insert(s);
				}
			}

			hasRead = true;
			for (int i = 0; i <= VS_VZ; i++) {
				if (head[i] < 0) error.addError("At least one required heading was missing (look for [sx|lat,sy|lon,sz|alt] [vx|trk,vy|gs,vz|vs])");
			}
		} // end ! hasRead

		// determine what type of data this is...
		int linetype = UNKNOWN;
		if (input.columnHasValue(head[NAME]) &&
				input.columnHasValue(head[LAT_SX]) &&
				input.columnHasValue(head[LON_SY]) &&
				input.columnHasValue(head[ALT_SZ]) &&
				input.columnHasValue(head[TRK_VX]) &&
				input.columnHasValue(head[GS_VY]) &&
				input.columnHasValue(head[VS_VZ]) &&
				input.columnHasValue(head[TM_CLK])) {
			linetype = STATE;
			if (input.columnHasValue(head[SZ2])) {
				linetype = POLY;
			}
		} else {
			error.addError("Invalid data line "+Fm0(input.lineNumber()));
		}


		string thisName = input.getColumnString(head[NAME]);
		if (equals(thisName,"\"")) {
			thisName = name;
		}

		Position ss;
		Velocity vv;
		double tm = 0.0;
		if (head[TM_CLK] >= 0) {
			tm = parseClockTime(input.getColumnString(head[TM_CLK]));
			if (lastTime > tm && equals(thisName,name)) {
				error.addWarning("Time not increasing from "+Fm4(lastTime)+" to "+Fm4(tm)+" for aircraft "+name+", skipping non-consecutive data.");
				continue;
			}
		}

		name = thisName;

		stateIndex = getIndex(name);
		if ( stateIndex < 0) {
			stateIndex = size();
			if (linetype == POLY) {
				states.push_back(GeneralState(name,  SimpleMovingPoly(), tm, containmentList.find(thisName) != containmentList.end()));
			} else {
				states.push_back(GeneralState(name, Position::INVALID(), Velocity::INVALID(), tm)); // need to replace later
			}
		}

		if (input.hasError()) {
			error.addError(input.getMessage());
			states.clear();
			break;
		}

		// the values are in the default units.
		if (latlon) {
			ss =  Position(LatLonAlt::mk(input.getColumn(head[LAT_SX], "deg"),
					input.getColumn(head[LON_SY], "deg"),
					input.getColumn(head[ALT_SZ], "ft")));
		} else {
			ss =  Position(Point::mk(
					input.getColumn(head[LAT_SX], "NM"),
					input.getColumn(head[LON_SY], "NM"),
					input.getColumn(head[ALT_SZ], "ft")));
		}

		if (trkgsvs) {
			vv = Velocity::mkTrkGsVs(
					input.getColumn(head[TRK_VX], "deg"),
					input.getColumn(head[GS_VY], "knot"),
					input.getColumn(head[VS_VZ], "fpm"));
		} else {
			vv = Velocity::mkVxyz(
					input.getColumn(head[TRK_VX], "knot"),
					input.getColumn(head[GS_VY],  "knot"),
					input.getColumn(head[VS_VZ],  "fpm"));
		}

		if (linetype == POLY) {
			double top = input.getColumn(head[SZ2], "ft");
			SimpleMovingPoly p = states[stateIndex].getPolygon();
			// replace existing entry if time does not match
			if (states[stateIndex].getTime() != tm) {
				p =  SimpleMovingPoly();
			}
			p.setTop(top);
			p.addVertex(ss, vv);
			states[stateIndex] = GeneralState(name,p,tm,containmentList.find(thisName) != containmentList.end());
		} else {
			// replace existing entry
			states[stateIndex] =  GeneralState(name, ss, vv, tm);
		}

		lastTime = tm;


	}

	// reset accuracy parameters to their previous values
	Constants::set_horizontal_accuracy(h);
	Constants::set_vertical_accuracy(v);
	Constants::set_time_accuracy(t);
}

int GeneralStateReader::altHeadings(const std::string& s1, const std::string& s2, const std::string& s3, const std::string& s4) const {
    int r = input.findHeading(s1);
    if (r < 0 && s2.compare("") != 0) {
      r = input.findHeading(s2);
    }
    if (r < 0 && s3.compare("") != 0) {
      r = input.findHeading(s3);
    }
    if (r < 0 && s4.compare("") != 0) {
      r = input.findHeading(s4);
    }
    return r;
}

int GeneralStateReader::altHeadings(const std::string& s1, const std::string& s2, const std::string& s3) const {
    return altHeadings(s1,s2,s3,"");
}

int GeneralStateReader::altHeadings(const std::string& s1, const std::string& s2) const {
    return altHeadings(s1,s2,"","");
}

double GeneralStateReader::parseClockTime(const std::string& s) const {
    double tm = 0.0;
    try {
      if (clock) {
        tm = Util::parse_time(s);
      } else {
        tm = input.getColumn(head[TM_CLK], "s");
      }
    } catch (std::runtime_error e) {
      error.addError("error parsing time at line "+Fm0(input.lineNumber()));
    }
    return tm;
}

int GeneralStateReader::getIndex(const std::string& name) const {
	for (int i=0; i < size(); i++) {
		GeneralState s = states[i];
		if (equals(s.getName(),name)) {
			return i;
		}
	}
	return -1;
}

int GeneralStateReader::altHeadings5(const std::string& s1, const std::string& s2, const std::string& s3, const std::string& s4, const std::string& s5) const {
	int r = input.findHeading(s1);
	if (r < 0 && !equals(s2,"")) {
		r = input.findHeading(s2);
	}
	if (r < 0 && !equals(s3,"")) {
		r = input.findHeading(s3);
	}
	if (r < 0 && !equals(s4,"")) {
		r = input.findHeading(s4);
	}
	if (r < 0 && !equals(s5,"")) {
		r = input.findHeading(s5);
	}
	return r;

}

int GeneralStateReader::size() const {
	return states.size();
}

int GeneralStateReader::numAircraft() const {
	int cnt = 0;
	for (int i = 0; i < (int) states.size(); i++) {
		if (states[i].hasPointMass()) cnt++;
	}
	return cnt;
}

int GeneralStateReader::numPolygons() const {
	int cnt = 0;
	for (int i = 0; i < (int) states.size(); i++) {
		if (states[i].hasPolygon()) cnt++;
	}
	return cnt;
}

GeneralState GeneralStateReader::getGeneralState(int ac) const {
	if (ac < 0 || ac >= size()) return  GeneralState();
	return states[ac].copy();
}

vector<GeneralState> GeneralStateReader::getGeneralStateList() const {
	vector<GeneralState> s;
	for (int i = 0; i < (int) states.size(); i++) {
		s.push_back(states[i].copy());
	}
	return s;
}

bool GeneralStateReader::isLatLon() const {
	return latlon;
}

std::string GeneralStateReader::getFilename() const {
	return fname;
}




std::string GeneralStateReader::toString() const {
	string rtn = "StateReader: ------------------------------------------------\n";
	for (int j = 0; j < (int) states.size(); j++) {
		rtn = rtn + states[j].toString() + "\n";
	}
	return rtn;
}

}
