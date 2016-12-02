/* 
 * PlanWriter
 *
 * Contact: Jeff Maddalon
 * 
 * Copyright (c) 2011-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */


#include "PlanWriter.h"
#include "ParameterData.h"
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
    
	PlanWriter::PlanWriter() : error("PlanWriter") {
		//error = new ErrorLog("PlanWriter");
        display_units = true;
        tcpColumns = false;
        precision = 6;
	}
	

	void PlanWriter::open(const std::string& filename) {
		fname = filename;
		if ( filename == "") {
			error.addError("No file specified");
			return;
		}
		close();

		std::ofstream fw;
		fw.open(filename.c_str());
		if ( fw.fail() ) {
			error.addError("File "+fname+" read protected or not found");
			return;
		}
		open(&fw);
	}


	void PlanWriter::open(std::ostream* writer) {
		//error = new ErrorLog("StateWriter(Writer)");
		if (writer == NULL) {
			error.addError("Null supplied for Writer in open()");
			return;
		}
		//fw = writer;
        output = SeparatedOutput(writer);
        first_line = true;
        lines = 0;
	}

    void PlanWriter::close() {
    	output.close();
//    	if (fw != NULL) {
//    		try {
//    			output.close();
//    			fw.close();
//    		} catch (IOException e) {
//    			error.addError("Exception on close(): "+e.getMessage());
//    		}
//        	fw = null;
//    	}
    }
	
	int PlanWriter::getPrecision() const {
		return precision;
	}

	void PlanWriter::setPrecision(int prec) {
		precision = prec;
	}


	void PlanWriter::setColumnDelimiterTab() {
		if (first_line) {
			output.setColumnDelimiterTab();
		}
	}

	void PlanWriter::setColumnDelimiterComma() {
		if (first_line) {
			output.setColumnDelimiterComma();
		}
	}

	void PlanWriter::setColumnDelimiterSpace() {
		if (first_line) {
			output.setColumnDelimiterSpace();
			output.setEmptyValue("0");
		}
	}

	void PlanWriter::addComment(const std::string& comment) {
		output.addComment(comment);
	}

	void PlanWriter::setParameters(const ParameterData& pr) {
		output.setParameters(pr);
	}
	
	void PlanWriter::clearParameters() {
		output.clearParameters();
	}

	void PlanWriter::writePlan(const Plan& p, bool tcpColumnsLocal) {
		if (first_line) {
			tcpColumns = tcpColumnsLocal;
			latlon = p.isLatLon();
			output.setOutputUnits(display_units);

			// Comments and parameters are handled by SeparatedOutput
			
			output.addHeading("name", "unitless");
			if (latlon) {
				output.addHeading("lat",  "deg");
				output.addHeading("lon",  "deg");
				output.addHeading("alt",  "ft");
			} else {
				output.addHeading("sx",   "NM");
				output.addHeading("sy",   "NM");
				output.addHeading("sz",   "ft");
			}
			output.addHeading("time", "s");
			if (tcpColumns) {
				output.addHeading("type", "unitless");
				output.addHeading("trk", "deg");
				output.addHeading("gs",  "knot");
				output.addHeading("vs",  "fpm");
				output.addHeading("tcp_trk", "unitless");
				output.addHeading("accel_trk", "deg/s");
				output.addHeading("tcp_gs", "unitless");
				output.addHeading("accel_gs", "m/s^2");
				output.addHeading("tcp_vs", "unitless");
				output.addHeading("accel_vs", "m/s^2");
				output.addHeading("radius", "NM");
				if (latlon) {
					output.addHeading("src_lat", "deg");
					output.addHeading("src_lon", "deg");
					output.addHeading("src_alt",  "ft");
				} else {
					output.addHeading("src_x",   "NM");
					output.addHeading("src_y",   "NM");
					output.addHeading("src_z",   "ft");
				}
				output.addHeading("src_time", "s");
			} 
			output.addHeading("label", "unitless");
			
			first_line = false;
		}
		
		for (int i = 0; i < p.size(); i++) {
			output.addColumn(p.getName());
			output.addColumn(p.point(i).toStringList(precision,tcpColumns));
			lines++;
			output.writeLine();
		}
	}	
	

	int PlanWriter::size() const {
		return lines;
	}

    bool PlanWriter::isLatLon() const {
    	return latlon;
    }
    
	
	// ErrorReporter Interface Methods

	bool PlanWriter::hasError() const {
		return error.hasError() || output.hasError();
	}
	bool PlanWriter::hasMessage() const {
		return error.hasMessage() || output.hasMessage();
	}
	std::string PlanWriter::getMessage() {
		return error.getMessage() + output.getMessage();
	}
	std::string PlanWriter::getMessageNoClear() const {
		return error.getMessageNoClear() + output.getMessageNoClear();
	}

}
