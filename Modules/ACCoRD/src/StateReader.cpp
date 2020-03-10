/* 
 * StateReader
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


#include "StateReader.h"
#include "Units.h"
//#include "UnitSymbols.h"
#include "ErrorLog.h"
#include "ErrorReporter.h"
#include "SeparatedInput.h"
#include "AircraftState.h"
#include "Position.h"
#include "Velocity.h"
#include "string_util.h"
#include "format.h"
#include "Constants.h"
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <map>
#include <algorithm>
#include <stdexcept>

namespace larcfm {
  
  using std::string;
  using std::vector;
  
  StateReader::StateReader(): error("StateReader()") {
    input.setCaseSensitive(false);            // headers & parameters are lower case
    hasRead = false;
    latlon = false;
    clock = false;
    trkgsvs = false;
    for (int i = 0; i < definedColumns; i++) {
    	head.push_back(-1);
    }
    interpretUnits = false;
  }
  
//  StateReader::StateReader(const string& filename): error("StateReader("+filename+")") {
//    std::ifstream in;
//
//    in.open(filename.c_str());
//    if ( in.fail() ) {
//      error.addError("File "+filename+" read protected or not found");
//      states.clear();
//      return;
//    }
//    SeparatedInput si(&in);
//    input = si;
//    input.setCaseSensitive(false);            // headers & parameters are lower case
//    loadfile();
//  }
//
//  StateReader::StateReader(SeparatedInput& si): error("StateReader(SeparatedInput)") {
//    input = si;
//    input.setCaseSensitive(false);            // headers & parameters are lower case
//    loadfile();
//  }
//
  void StateReader::open(const string& filename) {
    std::ifstream in;
    in.open(filename.c_str());
    if ( in.fail() ) {
      error.addError("File "+filename+" read protected or not found");
      states.clear();
      return;
    } 
    error = ErrorLog("StateReader("+filename+")");
    open(&in);
    in.close();
  }

  void StateReader::open(std::istream* in) {
    SeparatedInput si(in);
    si.setCaseSensitive(false);            // headers & parameters are lower case
    vector<string> params = input.getParametersRef().getKeyList();
    for (unsigned int i = 0; i < params.size(); i++) {
      si.getParametersRef().set(params[i], input.getParametersRef().getString(params[i]));
    }
    input = si;
    loadfile();
  }  
  
  ParameterData& StateReader::getParametersRef() {
	  return input.getParametersRef();
  }

  ParameterData StateReader::getParameters() const {
	  return input.getParameters();
  }

  void StateReader::updateParameterData(ParameterData& p) const {
	  p.copy(input.getParameters(), true);
  }

  void StateReader::loadfile() {
    hasRead = false;
    clock = true;
    interpretUnits = false;
    states.clear();
    string name = ""; // the current aircraft name
    double lastTime = -1000000; // time must be increasing
    int stateIndex = -1;

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
        trkgsvs = (altHeadings("trk","track") >= 0);
        
        head[NAME] =   altHeadings("name", "aircraft", "id");
        head[LAT_SX] = altHeadings("sx", "lat", "latitude");
        head[LON_SY] = altHeadings("sy", "lon", "long", "longitude");
        head[ALT_SZ] = altHeadings("sz", "alt", "altitude");
        head[TRK_VX] = altHeadings("trk", "vx", "track");
        head[GS_VY] = altHeadings("gs", "vy", "groundspeed", "groundspd");
        head[VS_VZ] = altHeadings("vs", "vz", "verticalspeed", "hdot");
        head[TM_CLK] = altHeadings("clock", "time", "tm", "st");
        
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

        if (this->getParametersRef().contains("filetype")) {
          string sval = this->getParametersRef().getString("filetype");
          if (!equalsIgnoreCase(sval, "state") && !equalsIgnoreCase(sval, "history")) {
            error.addError("Wrong filetype: "+sval);
            break;
          }
        }
        
		// add new user column headings
		for (int i = 0; i < input.size(); i++) {
			std::string hd = input.getHeading(i);
			if (!equals(hd,"")) {
				int headingindex = input.findHeading(hd);
				if (std::find(head.begin(), head.end(), headingindex) == head.end()) {
					head.push_back(headingindex);
				}
			}
		}


        hasRead = true;
        for (int i = 0; i <= VS_VZ; i++) {
          if (head[i] < 0) error.addError("At least one required heading was missing (look for [sx|lat,sy|lon,sz|alt] [vx|trk,vy|gs,vz|vs])");
        }
      } // end if != hasRead
      
//      string thisName = input.getColumnString(head[NAME]);
//
//      Position ss;
//      Velocity vv;
//      double tm = 0.0;
//      if (head[TM_CLK] >= 0) {
//        tm = getClock(input.getColumnString(head[TM_CLK]));
//      }
//
//      if ((!equals(thisName,name) || lastTime > getClock(input.getColumnString(head[TM_CLK])))
//          && thisName.compare("\"") != 0) {
//        name = thisName;
//        states.push_back(AircraftState(name));
//        stateIndex++;
//      }
//
//      if (stateIndex < 0) {
//        error.addError("Cannot find first aircraft");
//        states.clear();
//        break;
//      }
		string thisName = input.getColumnString(head[NAME]);
		if (thisName == "\"") {
			thisName = name;
		}

		Position ss;
		Velocity vv;
		double tm = 0.0;
		if (head[TM_CLK] >= 0) {
			tm = parseClockTime(input.getColumnString(head[TM_CLK]));
			if (lastTime > tm && thisName == name) {
				error.addWarning("Time not increasing from "+Fm4(lastTime)+" to "+Fm4(tm)+" for aircraft "+name+", skipping non-consecutive data.");
				continue;
			}
		}

		name = thisName;

		stateIndex = getIndex(name);
		if ( stateIndex < 0) {
			stateIndex = size();
			states.push_back(AircraftState(name));
		}

      
      if (input.hasError()) {
        error.addError(input.getMessage());
        states.clear();
        break;
      }
      
      if (latlon) {
        ss = Position(LatLonAlt::mk(input.getColumn(head[LAT_SX], "deg"),
        		input.getColumn(head[LON_SY], "deg"),
        		input.getColumn(head[ALT_SZ], "ft")));
      } else {
        ss = Position(Vect3(
        		input.getColumn(head[LAT_SX], "nmi"),
        		input.getColumn(head[LON_SY], "nmi"),
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
        		input.getColumn(head[GS_VY], "knot"),
        		input.getColumn(head[VS_VZ], "fpm"));
      }
      states[stateIndex].add(ss, vv, tm);
      

		// handle extra columns
		for (int i = definedColumns; i < (int) head.size(); i++) {
			int colnum = head[i];
			std::pair<int,int> newkey(stateIndex, colnum);
			extracolumnValues.erase(newkey); // remove old entry, if it exists
			if (input.columnHasValue(colnum)) {
				std::string str = input.getColumnString(colnum);
				double val = input.getColumn(colnum, NaN, false);
				bool bol = false;
				if (equalsIgnoreCase(str,"true")) {
					bol = true;
				}
				extracolumnValues[newkey] = Triple<double,bool,std::string>(val, bol, str); // add new entry, if there is anything
			}
		}


      lastTime = tm;
      
    }
    // reset accuracy parameters to their previous values
    Constants::set_horizontal_accuracy(h);
    Constants::set_vertical_accuracy(v);
    Constants::set_time_accuracy(t);

  }
  
  int StateReader::altHeadings(const string& s1, const string& s2, const string& s3, const string& s4) const {
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
  
  int StateReader::altHeadings(const string& s1, const string& s2, const string& s3) const {
    return altHeadings(s1,s2,s3,"");
  }
  
  int StateReader::altHeadings(const string& s1, const string& s2) const {
    return altHeadings(s1,s2,"","");
  }
  
  
  double StateReader::parseClockTime(const string& s) const {
    double tm = 0.0;
    try {
      if (clock) {
//        string patternStr = "[:]";
//        vector<string> fields2 = split(s, patternStr);
//        tm = getd(fields2[2]) + 60 * getd(fields2[1]) + 3600 * getd(fields2[0]);
        tm = Util::parse_time(s);
      } else {
        tm = input.getColumn(head[TM_CLK], "s");
      }
    } catch (std::runtime_error e) {
      error.addError("error parsing time at line "+Fm0(input.lineNumber()));
    }
    return tm;
  }
  
  int StateReader::getIndex(const string& name) const {
		for (int i=0; i < size(); i++) {
			AircraftState s = states[i];
			if (s.name() == name) {
				return i;
			}
		}
		return -1;
	}



  int StateReader::size() const {
    return states.size();
  }
  
  AircraftState StateReader::getAircraftState(int ac) const {
	if (ac < 0 || ac >= size()) return AircraftState("INVALID");
    return states[ac];
  }

  vector<AircraftState> StateReader::getAircraftStateList() const {
    return states;
  }
  
  Position StateReader::getPosition(int ac) const {
	  //fpln("size "+to_string(size()));
	if (ac < 0 || ac >= size()) return isLatLon() ? Position::ZERO_LL(): Position::ZERO_XYZ();
	return states[ac].positionLast();
  }
  
  Velocity StateReader::getVelocity(int ac) const {
	if (ac < 0 || ac >= size()) return Velocity::ZEROV();
	return states[ac].velocityLast();
  }
  
  string StateReader::getName(int ac) const {
	if (ac < 0 || ac >= size()) return "";
    return states[ac].name();
  }  
    
  double StateReader::getTime(int ac) const {
		if (ac < 0 || ac >= size()) return 0.0;
		return states[ac].timeLast();
	}


  bool StateReader::isLatLon() const {
	  return latlon;
  }

	/**
	 * Return a list of all user-defined columns in this reader.
	 */
	std::vector<std::string> StateReader::getExtraColumnList() const {
		std::vector<std::string> names;
		for (int i = definedColumns; i < (int) head.size(); i++) {
			names.push_back(input.getHeading(head[i]));
		}
		return names;
	}

	/**
	 * Return the units for a given column.  If no units were specified, then return "unspecified".  If the column name was not found, return the empty string.
	 * @param colname name of the column in question.
	 */
	std::string StateReader::getExtraColumnUnits(const std::string& colname) const {
		int col = input.findHeading(colname);
		if (col >= 0) {
			return input.getUnit(col);
		} else {
			return "";
		}
	}

	/**
	 * Return true if the given aircraft has data for the indicated column.
	 * @param ac
	 * @param colname
	 * @return
	 */
	bool StateReader::hasExtraColumnData(int ac, const std::string& colname) const {
		int colnum = input.findHeading(colname);
		return extracolumnValues.find(std::pair<int,int>(ac,colnum)) != extracolumnValues.end();
	}

	/**
	 * Returns the column value associated with a given aircraft, interpreted as a double, or NaN if there is no info
	 */
	double StateReader::getExtraColumnValue(int ac, const std::string& colname) const {
		int colnum = input.findHeading(colname);
		std::pair<int,int> key = std::pair<int,int>(ac,colnum);
		extraTblType::const_iterator it = extracolumnValues.find(key);
		if (it != extracolumnValues.end()) {
			return it->second.first;
		} else {
			return NaN;
		}
	}

	/**
	 * Returns the column value associated with a given aircraft, interpreted as a boolean, or false if there is no info
	 */
	bool StateReader::getExtraColumnBool(int ac, const std::string& colname) const {
		int colnum = input.findHeading(colname);
		std::pair<int,int> key = std::pair<int,int>(ac,colnum);
		extraTblType::const_iterator it = extracolumnValues.find(key);
		if (it != extracolumnValues.end()) {
			return it->second.second;
		} else {
			return false;
		}
	}

	/**
	 * Returns the column value associated with a given aircraft, interpreted as a string, or the empty string if there is no info
	 */
	std::string StateReader::getExtraColumnString(int ac, const std::string& colname) const {
		int colnum = input.findHeading(colname);
		std::pair<int,int> key = std::pair<int,int>(ac,colnum);
		extraTblType::const_iterator it = extracolumnValues.find(key);
		if (it != extracolumnValues.end()) {
			return it->second.third;
		} else {
			return "";
		}
	}


}
