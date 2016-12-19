/* 
 * PolyReader
 *
 * Note: if the accuracy parameters (horizontalAccuracy, verticalAccuracy, timeAccuracy) 
 * are changed in a file, ONLY THOSE will be interpreted here (and values will be re-set
 * afterwards). This is necessary to correctly read in a Plan.  They may need to be 
 * explicitly set later to do any manipulations of the Plans.
 *
 * Contact: George Hagen
 *
 * Copyright (c) 2011-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */


#include "PolyReader.h"
#include "PlanReader.h"
#include "Units.h"
#include "ErrorLog.h"
#include "ErrorReporter.h"
#include "SeparatedInput.h"
#include "Plan.h"
#include "string_util.h"
#include "format.h"
#include "Constants.h"
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <stdexcept>

namespace larcfm {

using std::string;
using std::vector;

PolyReader::PolyReader() {
	error = ErrorLog("PolyReader()");
//	  clock = false;
//	std::ifstream in;
//
//	in.open(filename.c_str());
//	if ( in.fail() ) {
//		error.addError("File "+filename+" read protected or not found");
//		plans.clear();
//		containment.clear();
//		paths.clear();
//		return;
//	}
//	input = SeparatedInput(&in);
//	input.setCaseSensitive(false);            // headers & parameters are lower case
//	loadfile();
}

//PolyReader::PolyReader(SeparatedInput& si) {
//	error = ErrorLog("PolyReader(SeparatedInput)");
//	input = si;
//	input.setCaseSensitive(false);            // headers & parameters are lower case
//	loadfile();
//}

void PolyReader::open(const string& filename) {
  std::ifstream in;

  in.open(filename.c_str());
  if ( in.fail() ) {
    error.addError("File "+filename+" read protected or not found");
    plans.clear();
    return;
  }
  PolyReader::open(&in);
}

void PolyReader::open(std::istream* si) {
  input = SeparatedInput(si);
  PolyReader::loadfile();
}


void PolyReader::loadfile() {
    input.setCaseSensitive(false);            // headers & parameters are lower case

	plans.clear();
	containment.clear();
	paths.clear();
	bool hasRead = false;
	bool latlon = true;
	bool tcpinfo = true;
	bool trkgsvs = true;
	clock = true;
	plans.reserve(10);
	paths.reserve(10);
	head.resize(NavPoint::TCP_OUTPUT_COLUMNS+3);
	string name = ""; // the current aircraft name
	int planIndex = -1;
	int pathIndex = -1;
	int containmentIndex = -1;
	vector<string> containmentList;
	bool containmentLine = false;
	//    double lastTime = -1000000; // time must be increasing


	//    bool calcpolyvels = true;
	//    bool morphpolys = true;
	PolyPath::PathMode pathmode = PolyPath::MORPHING;

	// save accuracy info in temp vars
	double h = Constants::get_horizontal_accuracy();
	double v = Constants::get_vertical_accuracy();
	double t = Constants::get_time_accuracy();

	//    bool kinematic = false;

	while (!input.readLine()) {
		// look for each possible heading
		if (!hasRead) {

			// process heading
			latlon = true;
			if (altHeadings("lat", "lon", "long", "latitude") < 0) {
				latlon = false;
			}
			if (altHeadings("trk", "v_trk", "track") < 0) {
				trkgsvs = false;
			}

			if (input.findHeading("clock") < 0) {
				clock = false;
			}

			head[NAME] =    altHeadings("name", "aircraft", "id");
			head[LAT_SX] =  altHeadings("sx", "lat", "latitude");
			head[LON_SY] =  altHeadings("sy", "lon", "long", "longitude");
			head[SZ] =      altHeadings5("sz", "alt", "altitude", "sz1", "alt1");
			head[TIME] =    altHeadings("clock", "time", "tm", "st");
			head[SZ2] =      altHeadings("sz2", "alt2", "altitude_top");
			head[LABEL] =   input.findHeading("label");

			head[TYPE] = 	input.findHeading("type");
			head[TCP_TRK] =	input.findHeading("tcp_trk");
			head[TCP_GS] = 	input.findHeading("tcp_gs");
			head[TCP_VS] = 	input.findHeading("tcp_vs");
			head[ACC_TRK] = altHeadings("acc_trk", "trk_accel", "accel_trk");
			head[ACC_GS] = 	altHeadings("acc_gs", "gs_accel", "accel_gs");
			head[ACC_VS] =	altHeadings("acc_vs", "vs_accel", "accel_vs");
			head[TRK] =	altHeadings("trk", "v_trk", "v_x","track");
			head[GS] =	altHeadings("gs", "v_gs", "v_y", "groundspeed");
			head[VS] =	altHeadings("vs", "v_vs", "v_z", "verticalspeed");
			head[SRC_LAT_SX] =	altHeadings("src_x", "src_sx", "src_lat", "src_latitude");
			head[SRC_LON_SY] =	altHeadings("src_y", "src_sy", "src_lon", "src_longitude");
			head[SRC_ALT] =		altHeadings("src_z", "src_sz", "src_alt", "src_altitude");
			head[SRC_TIME] =    altHeadings("src_clock", "src_time", "src_tm", "src_t");
			head[RADIUS] =    altHeadings("radius", "turn_radius", "R");

			head[ENDTIME] = altHeadings("endtime", "end_time", "poly_end");


			// make sure all tcp columns are defined
			if (	   head[TCP_TRK] < 0 || head[TCP_GS] < 0 || head[TCP_VS] < 0
					|| head[ACC_TRK] < 0 || head[ACC_GS] < 0 || head[ACC_VS] < 0
					|| head[TRK] < 0 || head[GS] < 0 || head[VS] < 0
					|| head[SRC_LAT_SX] < 0 || head[SRC_LON_SY] < 0 || head[SRC_ALT] < 0 || head[SRC_TIME] < 0) {
				if (	   head[TCP_TRK] >= 0 || head[TCP_GS] >= 0 || head[TCP_VS] >= 0
						|| head[ACC_TRK] >= 0 || head[ACC_GS] >= 0 || head[ACC_VS] >= 0
						|| head[SRC_LAT_SX] >= 0 || head[SRC_LON_SY] >= 0 || head[SRC_ALT] >= 0 || head[SRC_TIME] >= 0)
				{
					std::string missing = "";
					for (int i = TCP_TRK; i <= SRC_TIME; i++) {
						if (head[i] < 0) missing += " "+Fm0(i);
					}
					error.addWarning("Ignoring incorrect or incomplete TCP headers:"+missing);
				}
				tcpinfo = false;
			}

			// set accuracy parameters
			if (this->getParametersRef().contains("horizontalAccuracy")) {
				Constants::set_horizontal_accuracy(this->getParametersRef().getValue("horizontalAccuracy","m"));
			}
			if (this->getParametersRef().contains("verticalAccuracy")) {
				Constants::set_vertical_accuracy(this->getParametersRef().getValue("verticalAccuracy","m"));
			}
			if (this->getParametersRef().contains("timeAccuracy")) {
				Constants::set_time_accuracy(this->getParametersRef().getValue("timeAccuracy","s"));
			}

			if (this->getParametersRef().contains("containment")) {
				containmentList = getParametersRef().getListString("containment");
			}

			//        if (this->getParametersRef().contains("filetype")) {
			//          string sval = this->getParametersRef().getString("filetype");
			//          if (!equalsIgnoreCase(sval, "plan") && !equalsIgnoreCase(sval, "trajectory") &&
			//            !equalsIgnoreCase(sval, "poly") && !equalsIgnoreCase(sval, "plan+poly")) {
			//            error.addError("Wrong filetype: "+sval);
			//            break;
			//          }
			//        }

			if (this->getParametersRef().contains("pathMode")) {
				// note: does not generate error like Java does
				pathmode = PolyPath::parsePathMode(this->getParametersRef().getString("pathMode"));
				if ((pathmode == PolyPath::USER_VEL || pathmode == PolyPath::USER_VEL_FINITE) && (head[TRK] < 0 || head[GS] < 0 || head[VS] < 0)) {
					error.addError("Pathmode USER_VEL does not have velocity data, reverting to pathmode MORPHING");
					pathmode = PolyPath::MORPHING;
				}
			}

			//        if (this->getParameters().contains("plantype")) {
			//          string sval = this->getParameters().getString("plantype");
			//          if (equalsIgnoreCase(sval, "kinematic")) {
			//              kinematic = true;
			//          }
			//        }


			hasRead = true;
			for (int i = 0; i <= TIME; i++) {
				if (head[i] < 0) error.addError("This appears to be an invalid poly/plan file (missing header definitions) "+Fm0(i));
			}
		}

		// determine what type of data this is...
		int linetype = UNKNOWN;
		if (input.columnHasValue(head[NAME]) &&
				input.columnHasValue(head[LAT_SX]) &&
				input.columnHasValue(head[LON_SY]) &&
				input.columnHasValue(head[SZ]) &&
				input.columnHasValue(head[TIME])) {
			linetype = PLAN;
			if (input.columnHasValue(head[SZ2])) {
				linetype = POLY;
			}
		} else {
			error.addError("Invalid data line "+Fm0(input.lineNumber()));
		}

		string thisName = input.getColumnString(head[NAME]);
		double myTime = getClock(input.getColumnString(head[TIME]));

		if ((!equals(thisName,name) )//|| lastTime > myTime)
				&& !equals(thisName,"\"")) {
			if (linetype == POLY) {
				if (contains(containmentList,thisName)) {
					containmentLine = true;
					int i = containmentNameIndex(thisName);
					if (i >= 0 && equals(name, "\"")) {
						error.addWarning("Possible re-declaration of poly "+thisName);
					}
					name = thisName;
					if (i < 0 ){//|| lastTime > myTime) {
						containmentIndex = containment.size();
						PolyPath pp = PolyPath(name);
						pp.setPathMode(pathmode);
						containment.push_back(pp);
					} else {
						containmentIndex = i;
					}
				} else {
					containmentLine = false;
					int i = pathNameIndex(thisName);
					if (i >= 0 && equals(name, "\"")) {
						error.addWarning("Possible re-declaration of poly "+thisName);
					}
					name = thisName;
					if (i < 0 ){//|| lastTime > myTime) {
						pathIndex = paths.size();
						PolyPath pp = PolyPath(name);
						pp.setPathMode(pathmode);
						paths.push_back(pp);
					} else {
						pathIndex = i;
					}
				}
			} else if (linetype == PLAN) {
				int i = planNameIndex(thisName);
				if (i >= 0 && equals(name, "\"")) {
					error.addWarning("Possible re-declaration of aircraft "+thisName);
				}
				name = thisName;
				if (i < 0 ){//|| lastTime > myTime) {
					planIndex = plans.size();
					Plan nplan = Plan(name);
					if (getParametersRef().contains(name+"_note")) {
						nplan.setNote(getParametersRef().getString(name+"_note"));
					}
					plans.push_back(nplan);

				} else {
					planIndex = i;
				}
			}
		}

		if (planIndex < 0 && pathIndex < 0) {
			error.addError("Cannot find first entry");
			plans.clear();
			paths.clear();
			break;
		}

		if (input.hasError()) {
			error.addError(input.getMessage());
			plans.clear();
			paths.clear();
			break;
		}

		Position pos;
		if (latlon) {
			pos = Position::makeLatLonAlt(
					input.getColumn(head[LAT_SX], "deg"), "unspecified", // getColumn(_deg, head[LAT_SX]),
					input.getColumn(head[LON_SY], "deg"), "unspecified", // getColumn(_deg, head[LON_SY]),
					input.getColumn(head[SZ],      "ft"), "unspecified" // getColumn(_ft, head[SZ]),
			);
		} else {
			pos = Position::makeXYZ(
					input.getColumn(head[LAT_SX], "nmi"), "unspecified", // getColumn(_deg, head[LAT_SX]),
					input.getColumn(head[LON_SY], "nmi"), "unspecified", // getColumn(_deg, head[LON_SY]),
					input.getColumn(head[SZ],      "ft"), "unspecified"  // getColumn(_ft, head[SZ]),
			);
		}

		if (linetype == POLY) {
			double top = input.getColumn(head[SZ2], "ft");

			if (containmentLine) {
				containment[containmentIndex].addVertex(pos,top,myTime);

				if ((pathmode == PolyPath::USER_VEL || pathmode == PolyPath::USER_VEL_FINITE) && input.columnHasValue(head[TRK]) && input.columnHasValue(head[GS]) && input.columnHasValue(head[VS])) {
					Velocity vi = Velocity::makeTrkGsVs(input.getColumn(head[TRK]),input.getColumn(head[GS]),input.getColumn(head[VS]));
					containment[containmentIndex].setVelocity(containment[containmentIndex].getSegment(myTime), vi);
				}

				if (input.columnHasValue(head[ENDTIME])) {
					containment[containmentIndex] = containment[containmentIndex].truncate(input.getColumn(head[ENDTIME]));
				}

				if (containment[containmentIndex].hasError()) {
					error.addError(containment[containmentIndex].getMessage());
					containment.clear();
					break;
				} else if (containment[containmentIndex].hasMessage()) {
					error.addWarning(containment[containmentIndex].getMessage());
				}
			} else {
				paths[pathIndex].addVertex(pos,top,myTime);

				if ((pathmode == PolyPath::USER_VEL || pathmode == PolyPath::USER_VEL_FINITE) && input.columnHasValue(head[TRK]) && input.columnHasValue(head[GS]) && input.columnHasValue(head[VS])) {
					Velocity vi = Velocity::makeTrkGsVs(input.getColumn(head[TRK]),input.getColumn(head[GS]),input.getColumn(head[VS]));
					paths[pathIndex].setVelocity(paths[pathIndex].getSegment(myTime), vi);
				}

				if (input.columnHasValue(head[ENDTIME])) {
					paths[pathIndex] = paths[pathIndex].truncate(input.getColumn(head[ENDTIME]));
				}

				if (paths[pathIndex].hasError()) {
					error.addError(paths[pathIndex].getMessage());
					paths.clear();
					break;
				} else if (paths[pathIndex].hasMessage()) {
					error.addWarning(paths[pathIndex].getMessage());
				}
			}
		} else if (linetype == PLAN) {
			//        string mutString = "";
			//        if (input.columnHasValue(head[MUTABLE])) mutString = toLowerCase(input.getColumnString(head[MUTABLE])); // defined in SeparatedInput
			string label = "";
			if (input.columnHasValue(head[LABEL])) label = input.getColumnString(head[LABEL]);
			NavPoint n = NavPoint(
					pos,
					myTime,
					label
			);

			if (input.columnHasValue(head[TYPE])) {
				NavPoint::WayType ty = NavPoint::WayTypeValueOf(input.getColumnString(head[TYPE]));
				if (ty == NavPoint::UNKNOWN_WT) {
					error.addError("Unrecognized NavPoint WayType: "+input.getColumnString(head[TYPE]));
				} else if (ty == NavPoint::Virtual) {
					n = n.makeVirtual();
				} else if (ty == NavPoint::AltPreserve) {
					n = n.makeAltPreserve();
				}
			}

			if (input.columnHasValue(head[RADIUS])) {
				double rad = input.getColumn(head[RADIUS], "nmi");
				n = n.makeRadius(rad);
			}

			if (!tcpinfo) {
				NavPoint n2 = n.parseMetaDataLabel(label);
				if (n2.isInvalid()) {
					error.addError("Plan file uses invalid metadata format");
				} else {
					n = n2;
				}
			} else { // read tcp columns
				Position srcpos = Position::INVALID();
				if (input.columnHasValue(head[SRC_LAT_SX])) {
					if (latlon) {
						srcpos = Position::makeLatLonAlt(
								input.getColumn(head[SRC_LAT_SX], "deg"), "unspecified", // getColumn(_deg, head[LAT_SX]),
								input.getColumn(head[SRC_LON_SY], "deg"), "unspecified", // getColumn(_deg, head[LON_SY]),
								input.getColumn(head[SRC_ALT],      "ft"), "unspecified" // getColumn(_ft, head[SZ]),
						);
					} else {
						srcpos = Position::makeXYZ(
								input.getColumn(head[SRC_LAT_SX], "nmi"), "unspecified", // getColumn(_deg, head[LAT_SX]),
								input.getColumn(head[SRC_LON_SY], "nmi"), "unspecified", // getColumn(_deg, head[LON_SY]),
								input.getColumn(head[SRC_ALT],      "ft"), "unspecified"  // getColumn(_ft, head[SZ]),
						);
					}
				}
				double srcTime = input.getColumn(head[SRC_TIME], "s");
				n = n.makeSource(srcpos,  srcTime);
//				NavPoint np = n.makePosition(srcpos).makeTime(srcTime);
				Velocity vel = Velocity::INVALIDV();
				if (input.columnHasValue(head[TRK])) {
					if (trkgsvs) {
						vel = Velocity::makeTrkGsVs(
								input.getColumn(head[TRK], "deg"), "unspecified", // getColumn(_deg, head[LAT_SX]),
								input.getColumn(head[GS],  "kts"), "unspecified", // getColumn(_deg, head[LON_SY]),
								input.getColumn(head[VS],  "fpm"), "unspecified" // getColumn(_ft, head[SZ]),
						);
					} else {
						vel = Velocity::makeVxyz(
								input.getColumn(head[TRK], "kts"), // getColumn(_deg, head[LAT_SX]),
								input.getColumn(head[GS],  "kts"), "unspecified", // getColumn(_deg, head[LON_SY]),
								input.getColumn(head[VS],  "fpm"), "unspecified"  // getColumn(_ft, head[SZ]),
						);
					}
				}
				n = n.makeVelocityInit(vel);
				NavPoint::Trk_TCPType tcptrk = NavPoint::Trk_TCPTypeValueOf(input.getColumnString(head[TCP_TRK]));
				double acctrk = input.getColumn(head[ACC_TRK], "deg/s");
				double sRadius = 0.0;
				if (input.columnHasValue(head[RADIUS])) sRadius = input.getColumn(head[RADIUS], "NM");
				if (Util::almost_equals(sRadius,0.0)) sRadius = vel.gs()/acctrk;
				// TODO fix linearIndex
				int linearIndex = -1;
				switch (tcptrk) {
				case NavPoint::BOT: n = n.makeBOT(n.position(), n.time(), vel, sRadius, linearIndex); break;
				case NavPoint::EOT: n = n.makeEOT(n.position(), n.time(), vel, linearIndex); break;
				case NavPoint::EOTBOT: n = n.makeEOTBOT(n.position(), n.time(), vel, sRadius, linearIndex); break;
				case NavPoint::UNKNOWN_TRK: error.addError("Unrecognized Trk_TCPType: "+input.getColumnString(head[TCP_TRK])); break;
				default: break;// no change
				}
				NavPoint::Gs_TCPType tcpgs = NavPoint::Gs_TCPTypeValueOf(input.getColumnString(head[TCP_GS]));
				double accgs = input.getColumn(head[ACC_GS], "m/s^2");
				// TODO fix linearIndex
				linearIndex = -1;
				switch (tcpgs) {
				case NavPoint::BGS: n = n.makeBGS(n.position(), n.time(), accgs, vel, linearIndex); break;
				case NavPoint::EGS: n = n.makeEGS(n.position(), n.time(), vel, linearIndex); break;
				case NavPoint::EGSBGS: n = n.makeEGSBGS(n.position(), n.time(), accgs, vel, linearIndex); break;
				case NavPoint::UNKNOWN_GS: error.addError("Unrecognized Gs_TCPType: "+input.getColumnString(head[TCP_GS])); break;
				default: break;// no change
				}
				NavPoint::Vs_TCPType tcpvs = NavPoint::Vs_TCPTypeValueOf(input.getColumnString(head[TCP_VS]));
				double accvs = input.getColumn(head[ACC_VS], "m/s^2");
				// TODO fix linearIndex
				linearIndex = -1;
				switch (tcpvs) {
				case NavPoint::BVS: n = n.makeBVS(n.position(), n.time(), accvs, vel, linearIndex); break;
				case NavPoint::EVS: n = n.makeEVS(n.position(), n.time(), vel, linearIndex); break;
				case NavPoint::EVSBVS: n = n.makeEVSBVS(n.position(), n.time(), accvs, vel, linearIndex); break;
				case NavPoint::UNKNOWN_VS: error.addError("Unrecognized Vs_TCPType: "+input.getColumnString(head[TCP_VS])); break;
				default: break;// no change
				}
			}

			plans[planIndex].add(n);

			if (plans[planIndex].hasError()) {
				error.addError(plans[planIndex].getMessage());
				plans.clear();
				break;
			} else if (plans[planIndex].hasMessage()) {
				error.addWarning(plans[planIndex].getMessage());
			}
		}

		//      lastTime = myTime;
	} // while

	for (int i = 0; i < (signed)paths.size(); i++) {
		PolyPath path = paths[i];
		if (!path.validate())
			error.addError("Path "+Fm0(i)+" "+path.getName()+":"+path.getMessage());
	}

	for (int i = 0; i < (signed)containment.size(); i++) {
		PolyPath path = containment[i];
		if (!path.validate())
			error.addError("Containment area "+Fm0(i)+" "+path.getName()+":"+path.getMessage());
	}

	//	// check if we have kinematic plans with no tcps (this is not actually a problem, but is unlikely)
	//	if (kinematic) {
	//		for (int i = 0; i < (int)plans.size(); i++) {
	//			bool hastcp = false;
	//			for (int j = 0; j < plans[i].size(); j++) {
	//				if (plans[i].point(j).isTCP()) {
	//					hastcp = true;
	//					break;
	//				}
	//			}
	//			if (!hastcp) {
	//				error.addWarning("Plan "+plans[i].getName()+" is marked as kinematic but has no TCP data");
	//			}
	//		}
	//	}


	// reset accuracy parameters to their previous values
	Constants::set_horizontal_accuracy(h);
	Constants::set_vertical_accuracy(v);
	Constants::set_time_accuracy(t);

}


int PolyReader::pathNameIndex(const std::string& name) const {
	for (unsigned int i = 0; i < paths.size(); i++) {
		if (equals(paths[i].getName(), name))
			return i;
	}
	return -1;
}


int PolyReader::containmentNameIndex(const std::string& name) const {
	for (unsigned int i = 0; i < containment.size(); i++) {
		if (equals(containment[i].getName(), name))
			return i;
	}
	return -1;
}


int PolyReader::altHeadings5(const string& s1, const string& s2, const string& s3, const string& s4, const string& s5) const {
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
	if (r < 0 && s5.compare("") != 0) {
		r = input.findHeading(s5);
	}
	return r;
}

int PolyReader::planSize() const {
	return plans.size();
}

int PolyReader::polySize() const {
	return paths.size();
}

int PolyReader::containmentSize() const {
	return containment.size();
}

Plan PolyReader::getPlan(int i) const {
	return plans[i];
}

PolyPath PolyReader::getPolyPath(int i) const {
	return paths[i];
}

PolyPath PolyReader::getContainmentPolygon(int i) const {
	return containment[i];
}

int PolyReader::combinedSize() const {
	return plans.size() + paths.size();
}

Plan PolyReader::getCombinedPlan(int i) const {
	if (i < (signed)plans.size()) {
		return plans[i];
	} else {
		int j = i-plans.size();
		return paths[j].buildPlan().first;
	}
}

}
