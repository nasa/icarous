/* 
 * StateReader
 *
 * Contact: George Hagen
 * 
 * Copyright (c) 2011-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#include "ErrorLog.h"
#include "ErrorReporter.h"
#include "SeparatedInput.h"
#include "StateReader.h"
#include "SequenceReader.h"
#include "AircraftState.h"
#include "string_util.h"
#include "Position.h"
#include "Velocity.h"
#include "Constants.h"
#include "format.h"
#include <string>
#include <vector>
#include <set>
#include <map>
#include <iostream>
#include <algorithm>
#include <iostream>

namespace larcfm {
using std::string;
using std::vector;
using std::set;
using std::map;
using std::pair;
using std::cout;
using std::endl;

    /** A new, empty StateReader.  This may be used to store parameters, but nothing else. */
	SequenceReader::SequenceReader() {
		error = ErrorLog("SequenceReader(no file)");
		windowSize = AircraftState::DEFAULT_BUFFER_SIZE;
		input.setCaseSensitive(false);            // headers & parameters are lower case
	}

	
	SequenceReader::SequenceReader(const string& filename) {
		error = ErrorLog("SequenceReader("+filename+")");
		windowSize = AircraftState::DEFAULT_BUFFER_SIZE;
	    std::ifstream in;

	    in.open(filename.c_str());
	    if ( in.fail() ) {
	      error.addError("File "+filename+" read protected or not found");
	      return;
	    }
	    input = SeparatedInput(&in);
	    input.setCaseSensitive(false);            // headers & parameters are lower case
	    loadfile();
	}

	void SequenceReader::readFile(const string& filename) {
	    std::ifstream in;
	    in.open(filename.c_str());
	    if ( in.fail() ) {
	      error.addError("File "+filename+" read protected or not found");
	      sequenceTable.clear();
	      return;
	    }
	    error = ErrorLog("SequenceReader("+filename+")");
	    SeparatedInput si(&in);
	    si.setCaseSensitive(false);            // headers & parameters are lower case
	    vector<string> params = input.getParametersRef().getList();
	    for (unsigned int i = 0; i < params.size(); i++) {
	      si.getParametersRef().set(params[i], input.getParametersRef().getString(params[i]));
	    }
	    input = si;
	    loadfile();
	}
	
	
	void SequenceReader::loadfile() {

		hasRead = false;
		clock = true;
		//interpretUnits = false;
		sequenceTable.clear();
		states.clear();
		nameIndex.clear();
		names.clear();
		string lastName = ""; // the current aircraft name
		//double lastTime = -1000000; // time must be increasing
		int stateIndex = -1;

	    // save accuracy info in temp vars
	    double h = Constants::get_horizontal_accuracy();
	    double v = Constants::get_vertical_accuracy();
	    double t = Constants::get_time_accuracy();


	    while (!input.readLine()) {
	      // look for each possible heading
	      if (!hasRead) {

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
	          if (!equalsIgnoreCase(sval, "state") && !equalsIgnoreCase(sval, "history") && !equalsIgnoreCase(sval, "sequence")) {
	            error.addError("Wrong filetype: "+sval);
	            break;
	          }
	        }

	        hasRead = true;
	        for (int i = 0; i <= TM_CLK; i++) {
	          if (head[i] < 0) error.addError("This appears to be an invalid state file (missing header definitions)");
	        }
	      }

	      string thisName = input.getColumnString(head[NAME]);

			if (equals(thisName,"\"") && !equals(lastName,"")) {
				thisName = lastName;
			} else if ((equals(thisName,"\"") && equals(lastName,"")) || equals(thisName,"")) {
				error.addError("Cannot find first aircraft");
				sequenceTable.clear();
				break;
			} else {
				if (names.find(thisName) == names.end()) {
					lastName = thisName;
					nameIndex.push_back(thisName);
					names.insert(thisName);
					stateIndex++;
				}
			}


	      Position ss;
	      Velocity vv;
	      double tm = 0.0;
	      if (head[TM_CLK] >= 0) {
	        tm = parseClockTime(input.getColumnString(head[TM_CLK]));
	      }

		  SequenceEntry sequenceEntry;

		  if (sequenceTable.find(tm) != sequenceTable.end()) {
			  sequenceEntry = sequenceTable[tm];
		  }

			if (input.hasError()) {
				error.addError(input.getMessage());
				sequenceTable.clear();
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

//fpln("$#%%# "+thisName+"  "+ss.toString()+"  "+vv.toString()+"  "+Fm4(tm));

	      sequenceEntry[thisName] = pair<Position,Velocity>(ss,vv);
	      sequenceTable[tm] = sequenceEntry;
	      //lastTime = tm;
	    }
	    // reset accuracy parameters to their previous values
	    Constants::set_horizontal_accuracy(h);
	    Constants::set_vertical_accuracy(v);
	    Constants::set_time_accuracy(t);
        

        // we initially load the LAST sequent as the active one
        setLastActive();
	}


	/** Return the number of sequence entries in the file */
	int SequenceReader::sequenceSize() const {
		return sequenceTable.size();
	}
	
	/**
	 * Sets the window size for the active sequence set
	 * @param s > 0
	 */
	void SequenceReader::setWindowSize(int s) {
		if (s > 0) windowSize = s;
	}

	/**
	 * Returns the current window size
	 */
	int SequenceReader::getWindowSize() const {
		return windowSize;
	}
	
	// remove any time entries with only one aircraft
	void SequenceReader::clearSingletons() {
		vector<double> keys = sequenceKeys();
		for (int i = 0; i < (signed int)keys.size(); i++) {
			if (sequenceTable[keys[i]].size() < 2) {
//fpln("removing singleton at time "+Fm1(keys[i]));
				sequenceTable.erase(keys[i]);
			}
		}
	}

	// we need to preserve the order of the aircraft as in the input file (because the first might be the only way we know which is the ownship)
	// so we build an vector states to us as the subset of all possible inputs
	void SequenceReader::buildActive(double tm) {
		vector<double> times = sequenceKeysUpTo(windowSize,tm); // Note: this includes the last entry
		map<string, bool> included; // use to make sure there are no duplicates
		states.clear();
		// build all AircraftStates that exist in these times
		for (int i = 0; i < (signed int)nameIndex.size(); i++) { // work through the names in order
			string name = nameIndex[i];
			for (int j = 0; j < (signed int)times.size(); j++) { // for each name, work through the times in the window
				double time = times[j];
				SequenceEntry sequenceEntry = sequenceTable[time];
				if (sequenceEntry.find(name) != sequenceEntry.end()) {	// name has an entry at this time
					if (included.find(name) == included.end()) {  // name has not been added to the states list yet
						// build a new AircraftState
						included[name]=true;				// note name has been used
						states.push_back(AircraftState(name)); 	// add new
					}
					pair<Position,Velocity> p = sequenceEntry[name];	// get entry info
					states[states.size()-1].add(p.first, p.second, time); // we always work with the last added states entry, because they're ordered by name
//fpln(p.first.toString()+" "+p.second.toString()+Fm4(time));
				}
			}
		}
	}
	
	/**
	 * Given a sequence key, set the active set of states.  If no such key exists, the active set is left empty.
	 * @param tm Sequence key (time)
	 */
	void SequenceReader::setActive(double tm) {
		states.clear();
		if (sequenceTable.find(tm) != sequenceTable.end()) {
			buildActive(tm);
		}
	}
	
	/**
	 * Set the first entry to be the active one.
	 */
	void SequenceReader::setFirstActive() {
		vector<double> keys = sequenceKeys();
		if (keys.size() > 0)
			buildActive(keys[0]);
		else
			states.clear();
	}

	/**
	 * Set the last entry to be the active one.
	 */
	void SequenceReader::setLastActive() {
		vector<double> keys = sequenceKeys();
		if (keys.size() > 0)
			buildActive(keys[keys.size()-1]);
		else
			states.clear();
	}

	
	/**
	 *  Returns a sorted list of all sequence keys
	 */
	vector<double> SequenceReader::sequenceKeys() {
		vector<double> arl;
		SequenceEntryMap::iterator pos;
		for (pos = sequenceTable.begin(); pos != sequenceTable.end(); ++pos) {
			arl.push_back(pos->first);
		}
		sort(arl.begin(),arl.end());
		return arl;
	}

	/** a list of n > 0 sequence keys, stopping at the given time (inclusive) */ 
	vector<double> SequenceReader::sequenceKeysUpTo(int n, double tm) {
		vector<double> arl;
		SequenceEntryMap::iterator pos;
		for (pos = sequenceTable.begin(); pos != sequenceTable.end(); ++pos) {
			if (pos->first <= tm) {
				arl.push_back(pos->first);
			}
		}
		sort(arl.begin(),arl.end());
		// limit to window size
		while ((signed int)arl.size() > n) {
			arl.erase(arl.begin());
		}
		return arl;
	}

	/** Returns true if an entry exists for the given name and time */
	bool SequenceReader::hasEntry(const string& name, double time) {
		return sequenceTable.find(time) != sequenceTable.end() && sequenceTable[time].find(name) != sequenceTable[time].end();
	}

	/** Returns the Position entry for a given name and time.  If no entry for this name and time, returns a zero position and sets a warning. */
	Position SequenceReader::getSequencePosition(const string& name, double time) {
		if (hasEntry(name,time)) {
			return sequenceTable[time][name].first;
		} else {
			error.addWarning("getSequencePosition: invalid name/time combination");
			return Position::ZERO_LL();
		}
	}

	/** Returns the Velocity entry for a given name and time.  If no entry for this name and time, returns a zero velocity and sets a warning. */
	Velocity SequenceReader::getSequenceVelocity(const string& name, double time) {
		if (hasEntry(name,time)) {
			return sequenceTable[time][name].second;
		} else {
			error.addWarning("getSequenceVelocity: invalid name/time combination");
			return Velocity::ZEROV();
		}
	}

	void SequenceReader::setEntry(double time, const std::string& name, const Position& p, const Velocity& v) {
		if (sequenceTable.find(time) != sequenceTable.end()) {
			sequenceTable[time] = SequenceEntry();
		}
		sequenceTable[time][name] = pair<Position,Velocity>(p,v);
	}

	void SequenceReader::removeAircraft(const vector<string>& alist) {
		for (unsigned i = 0; i < alist.size(); i++) {
			string a = alist[i];
			vector<double> keys = sequenceKeys();
			for (unsigned j = 0; j < keys.size(); i++) {
				double key = keys[j];
				if (sequenceTable[key].find(a) != sequenceTable[key].end()) {
					sequenceTable[key].erase(a);
				}
			}
			if (names.find(a) != names.end()) {
				names.erase(a);
			}
			for (vector<string>::iterator pos = nameIndex.begin(); pos < nameIndex.end(); ++pos) {
				if (equals(*pos,a)) {
					nameIndex.erase(pos);
					break;
				}
			}
		}
		setLastActive();
	}

//    string SequenceReader::toString() const {
//	//return input.tostring();
//    	string rtn = "SequenceReader: ------------------------------------------------\n";
//    	vector<double> keys = sequenceKeys();
//    	if (keys.size() > 0) {
//    	  rtn = rtn+" Sequence start: "+keys.get(0)+"\n";
//    	  rtn = rtn+" Sequence end: "+keys.get(keys.size()-1)+"\n";
//    	  rtn = rtn + "Size: "+keys.size()+"\n";
//    	} else {
//    		rtn = rtn+" Empty sequence\n";
//    	}
//    	rtn = rtn+" ACTIVE:\n";
//    	for (int j = 0; j < states.size(); j++) {
//            rtn = rtn + states.get(j)+ "\n";
//    	}
//    	return rtn;
//    }


}

