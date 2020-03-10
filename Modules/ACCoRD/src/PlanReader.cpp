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
 * Copyright (c) 2011-2019 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */


#include "PlanReader.h"

#include "Units.h"
#include "ErrorLog.h"
#include "ErrorReporter.h"
#include "SeparatedInput.h"
#include "KinematicsPosition.h"
#include "Plan.h"
#include "PlanUtil.h"
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

PlanReader::PlanReader()  : error("PlanReader()") {
//	error = ErrorLog("PolyReader()");
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

//PolyReader::PolyReader(SeparatedInput& si)  // Not implemented!

void PlanReader::open(const string& filename) {
  std::ifstream in;

  in.open(filename.c_str());
  if ( in.fail() ) {
    error.addError("File "+filename+" read protected or not found");
    plans.clear();
    return;
  }
  PlanReader::open(&in);
  return;
}

void PlanReader::open(std::istream* si) {
  input = SeparatedInput(si);
//  timeOffset = 0.0;
  PlanReader::loadfile();
  return;
}


void PlanReader::loadfile() {
    input.setCaseSensitive(false);            // headers & parameters are lower case
	plans.clear();
	containment.clear();
	paths.clear();
	bool hasRead = false;
	bool latlon = true;
	bool tcp_data = true;
	bool trkgsvs = true;
	clock = true;
	plans.reserve(10); // note: reserve() is appropriate because we exclusively use push_back to populate the vectors
	paths.reserve(10);
	head.resize(TcpData::TCP_OUTPUT_COLUMNS+7); // MAKE SURE THIS MATCHES THE NUMBER OF COLUMNS!
	std::string name = ""; // the current aircraft name
	int planIndex = -1;
	int pathIndex = -1;
	int containmentIndex = -1;
	vector<string> containmentList;
	bool containmentLine = false;


	PolyPath::PathMode pathmode = PolyPath::MORPHING;

	// save accuracy info in temp vars
	double h = Constants::get_horizontal_accuracy();
	double v = Constants::get_vertical_accuracy();
	double t = Constants::get_time_accuracy();

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
			OLD_COLUMN_HEADERS = false;
			if (input.findHeading("name") == 0) {
				OLD_COLUMN_HEADERS = true;
			}

			if (OLD_COLUMN_HEADERS) {
				head[ID] = altHeadings("name", "aircraft", "id"); //TODO: name->label ?
				head[POINT_NAME] = altHeadings("label","point_name", "fix"); //TODO: label->name ?
				//error.addWarning("Deprecated file format: plan identifier column should now use 'ID' header.  'name' refers to navpoint names.");
			} else { // new format!
				head[ID] = altHeadings("aircraft", "id");
				head[POINT_NAME] = altHeadings("name","point_label","point_name", "fix");
			}

			head[LAT_SX] =  altHeadings("sx", "lat", "latitude");
			head[LON_SY] =  altHeadings("sy", "lon", "long", "longitude");
			head[SZ] =      altHeadings5("sz", "alt", "altitude", "sz1", "alt1");
			head[TIME] =    altHeadings("clock", "time", "tm", "st");
			head[SZ2] =     altHeadings("sz2", "alt2", "altitude_top");
			head[INFO] =		altHeadings("info", "information", "tcp_info");

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
			head[RADIUS] =    altHeadings("radius", "turn_radius", "R");
			head[CENTER_LAT_SX] = altHeadings("center_x", "center_lat");
			head[CENTER_LON_SY] = altHeadings("center_y", "center_lon");
			head[CENTER_ALT] =    altHeadings("center_z", "center_alt");

			head[ENDTIME] = altHeadings("endtime", "end_time", "poly_end");


			// make sure all tcp columns are defined
			if (	   head[TCP_TRK] < 0 || head[TCP_GS] < 0 || head[TCP_VS] < 0
					|| (head[RADIUS] < 0 && head[ACC_TRK] < 0)
					|| head[ACC_GS] < 0 || head[ACC_VS] < 0
					) {
				if (	   head[TCP_TRK] >= 0 || head[TCP_GS] >= 0 || head[TCP_VS] >= 0
						|| head[ACC_GS] >= 0 || head[ACC_VS] >= 0
						)
				{
					std::string missing = "";
					int i;
					i = TCP_TRK;
					if (head[i] < 0) missing += " "+Fm0(i);
					i = TCP_GS;
					if (head[i] < 0) missing += " "+Fm0(i);
					i = TCP_VS;
					if (head[i] < 0) missing += " "+Fm0(i);
					i = ACC_GS;
					if (head[i] < 0) missing += " "+Fm0(i);
					i = ACC_VS;
					if (head[i] < 0) missing += " "+Fm0(i);
					i = RADIUS;
					if (head[i] < 0) missing += " "+Fm0(i);
					error.addWarning("Ignoring incorrect or incomplete TCP headers:"+missing);
				}
				tcp_data = false;
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

			if (this->getParametersRef().contains("pathMode")) {
				// note: does not generate error like Java does
				pathmode = PolyPath::parsePathMode(this->getParametersRef().getString("pathMode"));
				if ((pathmode == PolyPath::USER_VEL || pathmode == PolyPath::USER_VEL_FINITE || pathmode == PolyPath::USER_VEL_EVER) && (head[TRK] < 0 || head[GS] < 0 || head[VS] < 0)) {
					error.addError("Pathmode USER_VEL does not have velocity data, reverting to pathmode MORPHING");
					pathmode = PolyPath::MORPHING;
				}
			}

			hasRead = true;
			for (int i = 0; i <= TIME; i++) {
				if (head[i] < 0) error.addError("This appears to be an invalid poly/plan file (missing header definitions) "+Fm0(i));
			}
		}
		// determine what type of data this is...
		int linetype = UNKNOWN;
		if (input.columnHasValue(head[ID]) &&
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

		string thisName = input.getColumnString(head[ID]);
		double myTime = getClock(input.getColumnString(head[TIME]));// + timeOffset;
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
						containmentIndex = static_cast<int>(containment.size());
						PolyPath pp = PolyPath(name);
						pp.setPathMode(pathmode);
						pp.setContainment(true);
						containment.push_back(pp);
						if (getParametersRef().contains(name+"_note")) {
							pp.setNote(getParametersRef().getString(name+"_note"));
						}
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
						pathIndex = static_cast<int>(paths.size());
						PolyPath pp = PolyPath(name);
						pp.setPathMode(pathmode);
						pp.setContainment(false);
						if (getParametersRef().contains(name+"_note")) {
							pp.setNote(getParametersRef().getString(name+"_note"));
						}
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
					input.getColumn(head[LAT_SX], "deg"), "rad", // getColumn(_deg, head[LAT_SX]),
					input.getColumn(head[LON_SY], "deg"), "rad", // getColumn(_deg, head[LON_SY]),
					input.getColumn(head[SZ],      "ft"), "m" // getColumn(_ft, head[SZ]),
			);
		} else {
			pos = Position::makeXYZ(
					input.getColumn(head[LAT_SX], "nmi"), "m", // getColumn(_deg, head[LAT_SX]),
					input.getColumn(head[LON_SY], "nmi"), "m", // getColumn(_deg, head[LON_SY]),
					input.getColumn(head[SZ],      "ft"), "m"  // getColumn(_ft, head[SZ]),
			);
		}
		if (linetype == POLY) {
			double top = input.getColumn(head[SZ2], "ft");
			double bottom = input.getColumn(head[SZ], "ft");

			if (containmentLine) {
				containment[containmentIndex].addVertex(pos,bottom,top,myTime);

				if ((pathmode == PolyPath::USER_VEL || pathmode == PolyPath::USER_VEL_FINITE || pathmode == PolyPath::USER_VEL_EVER)
				    && input.columnHasValue(head[TRK]) && input.columnHasValue(head[GS]) && input.columnHasValue(head[VS])) {
					Velocity vi = Velocity::makeTrkGsVs(input.getColumn(head[TRK], "deg"), "rad",
							input.getColumn(head[GS], "knot"), "m/s",
							input.getColumn(head[VS], "fpm"), "m/s");
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
				paths[pathIndex].addVertex(pos,bottom,top,myTime);

				if ((pathmode == PolyPath::USER_VEL || pathmode == PolyPath::USER_VEL_FINITE || pathmode == PolyPath::USER_VEL_EVER)
						&& input.columnHasValue(head[TRK]) && input.columnHasValue(head[GS]) && input.columnHasValue(head[VS])) {
					Velocity vi = Velocity::makeTrkGsVs(input.getColumn(head[TRK], "deg"), "rad",
							input.getColumn(head[GS], "knot"), "m/s",
							input.getColumn(head[VS], "fpm"), "m/s");
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
			string label = "";
			string info = "";
			if (input.columnHasValue(head[POINT_NAME])) label = input.getColumnString(head[POINT_NAME]);
			if (input.columnHasValue(head[INFO])) info = input.getColumnString(head[INFO]);
			NavPoint nnp = NavPoint(pos, myTime, label);
			TcpData n;
			if (! TcpData::motFlagInInfo) {
               if (contains(info,TcpData::MOTflag)) {  // upward compatibility with old file format
            	   n.setMOT(true);
            	   info = larcfm::replace(info,TcpData::MOTflag,"");
               }
			}
			n.setInformation(info);
			if (input.columnHasValue(head[TYPE])) {
				TcpData::WayType ty = TcpData::valueOfWayType(input.getColumnString(head[TYPE]));
				if (ty == TcpData::UNKNOWN_WT) {
					error.addError("Unrecognized NavPoint WayType: "+input.getColumnString(head[TYPE]));
				} else if (ty == TcpData::Virtual) {
					n = n.setVirtual();
				} else if (ty == TcpData::AltPreserve) {
					n = n.setAltPreserve();
				}
			}

			if (input.columnHasValue(head[RADIUS])) {
				double rad = input.getColumn(head[RADIUS], "nmi");
				n = n.setRadiusSigned(rad);
			}

			Position turnCenter = Position::INVALID();
			if (input.columnHasValue(head[CENTER_LAT_SX])) {
				if (latlon) {
					turnCenter = Position::makeLatLonAlt(
							input.getColumn(head[CENTER_LAT_SX], "deg"), "rad", 
							input.getColumn(head[CENTER_LON_SY], "deg"), "rad", 
							input.getColumn(head[CENTER_ALT],     "ft"), "m" 
							);
				} else {
					turnCenter = Position::makeXYZ(
							input.getColumn(head[CENTER_LAT_SX], "NM"), "m", 
							input.getColumn(head[CENTER_LON_SY], "NM"), "m", 
							input.getColumn(head[CENTER_ALT],    "ft"), "m"  
							);
				}
				n = n.setTurnCenter(turnCenter);
			}
			if (!tcp_data) {
				std::pair<TcpData,std::string> p = n.parseMetaDataLabel(label);
				TcpData n2 = p.first;
				if (n2.isInvalid()) {
					error.addError("Plan file uses invalid metadata format");
				} else {
					n=n2;
					nnp = nnp.makeName(p.second);
				}
			} else { // read tcp columns
				Velocity vel = Velocity::INVALIDV();
				if (input.columnHasValue(head[TRK])) {
					if (trkgsvs) {
						vel = Velocity::makeTrkGsVs(
								input.getColumn(head[TRK], "deg"), "rad", // getColumn(_deg, head[LAT_SX]),
								input.getColumn(head[GS],  "kts"), "m/s", // getColumn(_deg, head[LON_SY]),
								input.getColumn(head[VS],  "fpm"), "m/s" // getColumn(_ft, head[SZ]),
						);
					} else {
						vel = Velocity::makeVxyz(
								input.getColumn(head[TRK], "kts"), // getColumn(_deg, head[LAT_SX]),
								input.getColumn(head[GS],  "kts"), "m/s", // getColumn(_deg, head[LON_SY]),
								input.getColumn(head[VS],  "fpm"), "m/s"  // getColumn(_ft, head[SZ]),
						);
					}
				}
				TcpData::TrkTcpType tcptrk = TcpData::valueOfTrkType(input.getColumnString(head[TCP_TRK]));

				double sRadius = n.getRadiusSigned();
				if (Util::almost_equals(sRadius,0.0) && input.columnHasValue(head[ACC_TRK])) {
					double acctrk = input.getColumn(head[ACC_TRK], "deg/s");
					sRadius = vel.gs()/acctrk;
				}

				if ((equals(input.getColumnString(head[TCP_TRK]),"BOT") || equals(input.getColumnString(head[TCP_TRK]),"EOTBOT")) && turnCenter.isInvalid()) {
				   turnCenter = KinematicsPosition::centerFromRadius(pos, sRadius, vel.trk());
				}
				switch (tcptrk) {
				case TcpData::BOT: n = n.setBOT( sRadius, turnCenter); break;
				case TcpData::EOT: n = n.setEOT(); break;
				case TcpData::MOT: n = n.setMOT(true); break;
				case TcpData::EOTBOT: n = n.setEOTBOT( sRadius, turnCenter); break;
				case TcpData::UNKNOWN_TRK: error.addError("Unrecognized Trk_TCPType: "+input.getColumnString(head[TCP_TRK])); break;
				default: break;// no change
				}
				TcpData::GsTcpType tcpgs = TcpData::valueOfGsType(input.getColumnString(head[TCP_GS]));
				double accgs = input.getColumn(head[ACC_GS], "m/s^2");

				switch (tcpgs) {
				case TcpData::BGS: n = n.setBGS(accgs); break;
				case TcpData::EGS: n = n.setEGS( ); break;
				case TcpData::EGSBGS: n = n.setEGSBGS(accgs); break;
				case TcpData::UNKNOWN_GS: error.addError("Unrecognized Gs_TCPType: "+input.getColumnString(head[TCP_GS])); break;
				default: break;// no change
				}
				TcpData::VsTcpType tcpvs = TcpData::valueOfVsType(input.getColumnString(head[TCP_VS]));
				double accvs = input.getColumn(head[ACC_VS], "m/s^2");
				switch (tcpvs) {
				case TcpData::BVS: n = n.setBVS(accvs); break;
				case TcpData::EVS: n = n.setEVS( ); break;
				case TcpData::EVSBVS: n = n.setEVSBVS(accvs); break;
				case TcpData::UNKNOWN_VS: error.addError("Unrecognized Vs_TCPType: "+input.getColumnString(head[TCP_VS])); break;
				default: break;// no change
				}
				if (!n.isDefined()) {
					error.addError("Line "+Fm0(input.lineNumber())+": missing necessary TcpData information");
				}
			}
			plans[planIndex].add(nnp,n);

			if (plans[planIndex].hasError()) {
				error.addError(plans[planIndex].getMessage());
				plans.clear();
				break;
			} else if (plans[planIndex].hasMessage()) {
				error.addWarning(plans[planIndex].getMessage());
			}
		}
	} // while
	for (int i = 0; i < (signed)paths.size(); i++) {
		PolyPath path = paths[i];
		if (!path.validate())
			error.addError("Path "+Fm0(i)+" "+path.getID()+":"+path.getMessage());
	}

	for (int i = 0; i < (signed)containment.size(); i++) {
		PolyPath path = containment[i];
		if (!path.validate())
			error.addError("Containment area "+Fm0(i)+" "+path.getID()+":"+path.getMessage());
	}
	// reset accuracy parameters to their previous values
	Constants::set_horizontal_accuracy(h);
	Constants::set_vertical_accuracy(v);
	Constants::set_time_accuracy(t);
	return;
}// loadfile


int PlanReader::pathNameIndex(const std::string& name) const {
	for (unsigned int i = 0; i < paths.size(); i++) {
		if (equals(paths[i].getID(), name))
			return i;
	}
	return -1;
}


int PlanReader::containmentNameIndex(const std::string& name) const {
	for (unsigned int i = 0; i < containment.size(); i++) {
		if (equals(containment[i].getID(), name))
			return i;
	}
	return -1;
}

int PlanReader::planNameIndex(const std::string& name) const {
		for (unsigned int i = 0; i < plans.size(); i++) {
			if (equals(plans[i].getID(), name))
				return i;
		}
		return -1;
 }




int PlanReader::altHeadings5(const string& s1, const string& s2, const string& s3, const string& s4, const string& s5) const {
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

int PlanReader::planSize() const {
	return static_cast<int>(plans.size());
}

int PlanReader::polySize() const {
	return static_cast<int>(paths.size());
}

int PlanReader::containmentSize() const {
	return static_cast<int>(containment.size());
}

Plan PlanReader::getPlan(int i) const {
	return plans[i];
}

PolyPath PlanReader::getPolyPath(int i) const {
	return paths[i];
}

PolyPath PlanReader::getContainmentPolygon(int i) const {
	return containment[i];
}

int PlanReader::combinedSize() const {
	return static_cast<int>(plans.size() + paths.size());
}

Plan PlanReader::getCombinedPlan(int i) const {
	if (i < static_cast<int>(plans.size())) {
		return plans[i];
	} else {
		int j = i-static_cast<int>(plans.size());
		return paths[j].buildPlan().first;
	}
}


int PlanReader::altHeadings(const string& s1, const string& s2, const string& s3, const string& s4) const {
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

int PlanReader::altHeadings(const string& s1, const string& s2, const string& s3) const {
  return altHeadings(s1,s2,s3,"");
}

int PlanReader::altHeadings(const string& s1, const string& s2) const {
  return altHeadings(s1,s2,"","");
}




double PlanReader::getClock(const string& s) const {
  double tm = 0.0;
  try {
    if (clock) {
      string patternStr = "[:]";
      vector<string> fields2 = split(s, patternStr);
      tm = Util::parse_double(fields2[2]) + 60 * Util::parse_double(fields2[1]) + 3600 * Util::parse_double(fields2[0]);
    } else {
      tm = input.getColumn(head[TIME],"s");
    }
  } catch (std::runtime_error e) {
    error.addError("error parsing time at line "+Fm0(input.lineNumber()));
  }
  return tm;
}


int PlanReader::size() const {
  return static_cast<int>(plans.size());
}

ParameterData& PlanReader::getParametersRef() {
	  return input.getParametersRef();
}

ParameterData PlanReader::getParameters() const {
	  return input.getParameters();
}

void PlanReader::updateParameterData(ParameterData& p) const {
	  return p.copy(input.getParameters(),true);
}


}
