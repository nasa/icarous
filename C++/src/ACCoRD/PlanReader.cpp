/* 
 * PlanReader
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
 *
 */

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
  
  PlanReader::PlanReader(): error("PlanReader()") {
//	  hasRead = false;
//	  latlon = false;
//	  clock = false;
//	  input.setCaseSensitive(false);            // headers & parameters are lower case
  }
  
  void PlanReader::open(const string& filename) {
    std::ifstream in;
    
    in.open(filename.c_str());
    if ( in.fail() ) {
      error.addError("File "+filename+" read protected or not found");
      plans.clear();
      return;
    } 
    open(&in);
  }
  
  void PlanReader::open(std::istream* si) {
    input = SeparatedInput(si);
    loadfile();
  }
  
  void PlanReader::loadfile() {
    plans.clear();
    bool hasRead = false;
    clock = true;
    bool latlon = true;
    bool tcpinfo = true;
    bool trkgsvs = true;
    plans.reserve(10);
    head.resize(NavPoint::TCP_OUTPUT_COLUMNS+1);
    string name = ""; // the current aircraft name
//    double lastTime = -1000000; // time must be increasing
    int planIndex = -1;

    input.setCaseSensitive(false);

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
        head[SZ] =      altHeadings("sz", "alt", "altitude");
        head[TIME] =    altHeadings("clock", "time", "tm", "st");
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

//        if (this->getParametersRef().contains("filetype")) {
//          string sval = this->getParametersRef().getString("filetype");
//          if (!equalsIgnoreCase(sval, "plan") && !equalsIgnoreCase(sval, "trajectory")) {
//            error.addError("Wrong filetype: "+sval);
//            break;
//          }
//        }
        
        hasRead = true;
        for (int i = 0; i <= TIME; i++) {
          if (head[i] < 0) error.addError("This appears to be an invalid plan file (missing header definitions)");
        }
      }
      
      string thisName = input.getColumnString(head[NAME]);
      double myTime = getClock(input.getColumnString(head[TIME]));
      
      if ((!equals(thisName,name) )//|| lastTime > myTime)
          && thisName.compare("\"") != 0) {
    	int i = planNameIndex(thisName);
		if (i >= 0 && name.compare("\"")) {
			error.addWarning("Possible re-declaration of aircraft "+thisName);
		}
		name = thisName;
    	if (i < 0){ // || lastTime > myTime) {
    		planIndex = plans.size();
    		Plan nplan = Plan(name);
			// add note, if present
			if (getParametersRef().contains(name+"_note")) {
				nplan.setNote(getParametersRef().getString(name+"_note"));
			}
    		plans.push_back(nplan);
    	} else {
    		planIndex = i;
    	}
      }
      
      if (planIndex < 0) {
        error.addError("Cannot find first aircraft");
        plans.clear();
        break;
      }
      
      if (input.hasError()) {
        error.addError(input.getMessage());
        plans.clear();
        break;
      }
      
//      string mutString = "";
//      if (input.columnHasValue(head[MUTABLE])) mutString = toLowerCase(input.getColumnString(head[MUTABLE])); // defined in SeparatedInput

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
//			NavPoint np = n.makePosition(srcpos).makeTime(srcTime);
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
			NavPoint::Gs_TCPType tcpgs = NavPoint::Gs_TCPTypeValueOf(input.getColumnString(head[TCP_GS]));
			NavPoint::Vs_TCPType tcpvs = NavPoint::Vs_TCPTypeValueOf(input.getColumnString(head[TCP_VS]));
			double acctrk = input.getColumn(head[ACC_TRK], "deg/s");
			double sRadius = 0.0;
			if (input.columnHasValue(head[sRadius])) sRadius = input.getColumn(head[RADIUS], "NM");
			if (Util::almost_equals(sRadius,0.0)) sRadius = vel.gs()/acctrk;

			double accgs = input.getColumn(head[ACC_GS], "m/s^2");
			double accvs = input.getColumn(head[ACC_VS], "m/s^2");
			// TODO: fix linearIndex
			int linearIndex = -1;
			switch (tcptrk) {
			case NavPoint::BOT: n = n.makeBOT(n.position(), n.time(), vel, sRadius, linearIndex); break;
			case NavPoint::EOT: n = n.makeEOT(n.position(), n.time(), vel, linearIndex); break;
			case NavPoint::EOTBOT: n = n.makeEOTBOT(n.position(), n.time(), vel, sRadius, linearIndex); break;
			case NavPoint::UNKNOWN_TRK: error.addError("Unrecognized Trk_TCPType: "+input.getColumnString(head[TCP_TRK])); break;
			default: break;// no change
			}
			// TODO: fix linearIndex
			linearIndex = -1;
			switch (tcpgs) {
			case NavPoint::BGS: n = n.makeBGS(n.position(), n.time(), accgs, vel, linearIndex); break;
			case NavPoint::EGS: n = n.makeEGS(n.position(), n.time(), vel, linearIndex); break;
			case NavPoint::EGSBGS: n = n.makeEGSBGS(n.position(), n.time(), accgs, vel, linearIndex); break;
			case NavPoint::UNKNOWN_GS: error.addError("Unrecognized Gs_TCPType: "+input.getColumnString(head[TCP_GS])); break;
			default: break;// no change
			}
			// TODO: fix linearIndex
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

//	// check if we have kinematic plans with no tcps (this is not actually a problem, but is unlikely)
//	if (kinematic) {
//		for (int i = 0; i < (int)plans.size(); i++) {
//			bool hastcp = false;
//			for (int j = 0; j < (int)plans[i].size(); j++) {
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
  
  int PlanReader::planNameIndex(const std::string& name) const {
		for (unsigned int i = 0; i < plans.size(); i++) {
			if (equals(plans[i].getName(), name))
				return i;
		}
		return -1;
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
  
  NavPoint PlanReader::setTcpType(const NavPoint& n, const string& s) {
    if (equalsIgnoreCase(s,"bot")) return n.makeBOT(n.position(), n.time(),  n.velocityInit(), n.signedRadius(), n.linearIndex());
    else if(equalsIgnoreCase(s,"eot")) return n.makeEOT(n.position(), n.time(), n.velocityInit(), n.linearIndex());
     else return n;
  }

  
  int PlanReader::size() const {
    return plans.size();
  }
  
  Plan PlanReader::getPlan(int i) const {
    return plans[i];
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


//  bool PlanReader::containsParameter(const string& key) const {
//    return input.containsParameter(key);
//  }
//
//  string PlanReader::getParameterString(const string& key) const {
//    return input.getParameterString(key);
//  }
//
//  bool PlanReader::getParameterBool(const string& key) const {
//    return input.getParameterBool(key);
//  }
//
//  double PlanReader::getParameterValue(const string& key) const {
//    return input.getParameterValue(key);
//  }
//
//  double PlanReader::getParameterValue(const std::string& key, const std::string& defaultUnit) const {
//    return input.getParameterValue(key, defaultUnit);
//  }
//
//  int PlanReader::getParameterInt(const std::string& key) const {
//	  return (int)input.getParameterValue(key);
//  }
//
//  long PlanReader::getParameterLong(const std::string& key) const {
//	  return (long)input.getParameterValue(key);
//  }
//
//  std::string PlanReader::getParameterUnit(const std::string& key) const {
//	  return input.getParameterUnit(key);
//  }
//
//  void PlanReader::setParameter(const std::string& key, double value) {
//	    std::stringstream temp;
//	    temp.precision(20);
//	    temp << value << " [internal]";
//	    input.setParameter(key, temp.str());
//  }
//
//  void PlanReader::setParameter(const std::string& key, double value, const std::string& units) {
//	    std::stringstream temp;
//	    temp.precision(20);
//	    temp << value << " " << units;
//	    input.setParameter(key, temp.str());
//  }
//
//  void PlanReader::setParameter(const std::string& key, bool value) {
//	    std::stringstream temp;
//	    temp << value << "";
//	    input.setParameter(key, temp.str());
//  }
//
//  void PlanReader::setParameter(const std::string& key, const std::string& value) {
//	    input.setParameter(key, value);
//  }
//
//  void PlanReader::setParameter(const std::string& s) {
//	    input.setParameter(s);
//  }
//
//
//  vector<string> PlanReader::getParameterList() const {
//    return input.getParameterList();
//  }
//
//  void PlanReader::validateParameters(vector<string> c) const {
//    c.push_back("filetype");
//    c.push_back("plantype");
//    c.push_back("horizontalAccuracy");
//    c.push_back("verticalAccuracy");
//    c.push_back("timeAccuracy");
//    vector<string> u = input.unrecognizedParameters(c);
//    if (u.size() > 0) {
//      string s = "";
//      for (unsigned int i = 0; i < u.size(); ++i) {
//        s = s + " " + u[i];
//      }
//      error.addWarning("Invalid parameters: "+s);
//    }
//  }
//
//  void PlanReader::copyParameters(const ParameterReader& p, bool overwrite) {
//		vector<string> plist = p.getParameterList();
//		for (unsigned int i = 0; i < plist.size(); i++) {
//			string key = plist[i];
//			if (overwrite || !containsParameter(key)) {
//				input.setParameter(key, p.getParameterString(key));
//			}
//		}
//  }

}
