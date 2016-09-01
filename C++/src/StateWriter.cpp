/* 
 * StateWriter
 *
 * Contact: Jeff Maddalon
 * 
 * Copyright (c) 2011-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */


#include "StateWriter.h"
#include <cstdio>
#include <iostream>
#include <sstream>

namespace larcfm {

  using std::string;
  using std::vector;

  const double StateWriter::default_time = 0.0;

 	StateWriter::StateWriter() : error("StateWriter"), output(NULL) {
		//error = new ErrorLog("StateWriter");
        trkgsvs = true;
        velocity = true;
        display_time = true;
        display_units = true;
        precision = 6;
        first_line = false;
        latlon = false;
        lines = 0;
        fw = NULL;
	}
	
 	void StateWriter::open(const std::string& filename) {
	    std::ofstream out;
	    out.open(filename.c_str());
	    if ( out.fail() ) {
	      error.addError("File "+filename+" write protected");
	      return;
	    }
	    open(&out);
	}

	void StateWriter::open(std::ostream* writer) {
		error = ErrorLog("StateWriter(Writer)");
		if (writer == NULL) {
			error.addError("Null supplied for Writer in open()");
			return;
		}
		fw = writer;
        output = SeparatedOutput(writer);
        first_line = true;
        lines = 0;
	}

    void StateWriter::close() {
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
    		//} catch (std::bad_cast& e) {
    			// don't think it can ever reach here
    		}
    		fw = NULL;
    	}
    }
	
	bool StateWriter::isVelocity() const {
		return velocity;
	}

	void StateWriter::setVelocity(bool vel) {
		velocity = vel;
	}

	bool StateWriter::isTrkGsVs() const {
		return trkgsvs;
	}

	void StateWriter::setTrkGsVs(bool trk_gs_vs) {
		trkgsvs = trk_gs_vs;
	}

	int StateWriter::getPrecision() const {
		return precision;
	}

	void StateWriter::setPrecision(int precision_digits) {
		precision = precision_digits;
	}

	bool StateWriter::isOutputTime() const {
		return display_time;
	}

	void StateWriter::setOutputTime(bool displaytime) {
		display_time = displaytime;
	}

	/** Will the units be displayed? */
	bool StateWriter::isOutputUnits() const {
		return display_units;
	}

	/** Should the units be displayed? */
	void StateWriter::setOutputUnits(bool displayunits) {
		display_units = displayunits;
	}
	
	void StateWriter::setColumnDelimiterTab() {
		if (first_line) {
			output.setColumnDelimiterTab();
		}
	}

	void StateWriter::setColumnDelimiterComma() {
		if (first_line) {
			output.setColumnDelimiterComma();
		}
	}

	void StateWriter::setColumnDelimiterSpace() {
		if (first_line) {
			output.setColumnDelimiterSpace();
			output.setEmptyValue("0");
		}
	}

	void StateWriter::addComment(std::string comment) {
		output.addComment(comment);
	}

	void StateWriter::setParameters(ParameterData pr) {
		output.setParameters(pr);
	}

//	void StateWriter::setParameters(ParameterData pr, std::vector<std::string> list) {
//		output.setParameters(pr,list);
//	}

	void StateWriter::writeState(const std::string& name, double time, const Position& p, const Velocity& v) {
		if (first_line) {
			latlon = p.isLatLon();
			output.setOutputUnits(display_units);

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
			
			first_line = false;
		}
		output.addColumn(name);
		if (display_time) {
			output.addColumn(FmPrecision(time,precision));			
		}
		output.addColumn(p.toStringList(precision));
		if (velocity) {
			if (trkgsvs) {
				output.addColumn(v.toStringList(precision));							
			} else {
				output.addColumn(v.toStringXYZList(precision));											
			}
		}
		lines++;
		output.writeLine();		
	}	
	

	void StateWriter::writeState(const std::string& name, double time, const Position& p) {
		writeState(name,time,p,Velocity::ZEROV);
	}
	
	void StateWriter::writeState(const std::string& name, double time, const std::pair<Position,Velocity>& pv) {
		writeState(name, time, pv.first, pv.second);
	}
	
	void StateWriter::writeState(const std::string& name, const Position& p, const Velocity& v) {
		writeState(name, default_time, p, v);
	}
	
	void StateWriter::writeState(const std::string& name, const Position& p) {
		writeState(name, default_time, p);
	}
	
	/** Return the number of states added to the file */
	int StateWriter::size() const {
		return lines;
	}

    bool StateWriter::isLatLon() const {
    	return latlon;
    }
    
    std::string StateWriter::toString() const {
    	std::string rtn = "StateWriter: ------------------------------------------------\n";
//    	for (int j = 0; j < states.size(); j++) {
//            rtn = rtn + states.get(j)+ "\n";
//    	}
    	return rtn;
    }
	
	// ErrorReporter Interface Methods

	bool StateWriter::hasError() const {
		return error.hasError() || output.hasError();
	}
	bool StateWriter::hasMessage() const {
		return error.hasMessage() || output.hasMessage();
	}
	std::string StateWriter::getMessage() {
		return error.getMessage() + output.getMessage();
	}
	std::string StateWriter::getMessageNoClear() const {
		return error.getMessageNoClear() + output.getMessageNoClear();
	}


}
