/* 
 * PlanWriter
 *
 * Contact: Jeff Maddalon
 * 
 * Copyright (c) 2011-2017 United States Government as represented by
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
#include "PolyPath.h"
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
        mode = PolyPath::MORPHING;
        polyColumns = false;
        trkgsvs = true;
	}
	

	void PlanWriter::open(const std::string& filename) {
		fname = filename;
		if ( filename == "") {
			error.addError("No file specified");
			return;
		}
		close();

		//std::ofstream fw;
		fw.open(filename.c_str()); // ,std::ofstream::out);
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

	void PlanWriter::setPolyPathMode(PolyPath::PathMode m) {
		polyColumns = true;
		mode = m;
	}

	void PlanWriter::clearPolyPathMode() {
		polyColumns = false;
		mode = PolyPath::MORPHING;
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
		//fpln(" $$$$$$$$ writePlan: first_line = "+bool2str(first_line));
		if (first_line) {
			tcpColumns = tcpColumnsLocal;
			latlon = p.isLatLon();
			output.setOutputUnits(display_units);

			// Comments and parameters are handled by SeparatedOutput

			if (polyColumns) {
				output.setParameter("PathMode", PolyPath::pathModeToString(mode));
			}

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
				//output.addHeading("trk", "deg");
				//output.addHeading("gs",  "knot");
				//output.addHeading("vs",  "fpm");
				output.addHeading("tcp_trk", "unitless");
				output.addHeading("tcp_gs", "unitless");
				output.addHeading("tcp_vs", "unitless");
				output.addHeading("radius", "NM");
				output.addHeading("accel_gs", "m/s^2");
				output.addHeading("accel_vs", "m/s^2");
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
				if (latlon) {
					output.addHeading("center_lat", "deg");
					output.addHeading("center_lon", "deg");
					output.addHeading("center_alt",  "ft");
				} else {
					output.addHeading("center_x",   "NM");
					output.addHeading("center_y",   "NM");
					output.addHeading("center_z",   "ft");
				}
				output.addHeading("info", "unitless");
			}
			output.addHeading("label", "unitless");
			if (polyColumns) {
				if (latlon) {
					output.addHeading("alt2", "ft");//21
				} else {
					output.addHeading("sz2", "ft");//21
				}
				if (mode == PolyPath::USER_VEL || mode == PolyPath::USER_VEL_FINITE) {
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
			}


			first_line = false;
		}
		for (int i = 0; i < p.size(); i++) {
			output.addColumn(p.toStringList(i,precision,tcpColumns));
			if (polyColumns) {
				output.addColumn("-"); //alt2
				if (mode == PolyPath::USER_VEL || mode == PolyPath::USER_VEL_FINITE) {
					output.addColumn("-"); //velocity
					output.addColumn("-"); //velocty
					output.addColumn("-"); //velocity
				}
			}
			lines++;
			output.writeLine();
		}

	}	
	
	void PlanWriter::writePolyPath(const PolyPath& p, bool write_tcp) {
		//f.pln(" $$$ writePlan: p.size() = "+p.size());
		if (polyColumns) {
			if (first_line) {
				tcpColumns = write_tcp;
				if (!polyColumns) {
					mode = p.getPathMode();
				}
				polyColumns = true;
				latlon = p.isLatLon();
				output.setOutputUnits(display_units);

				// Comments and parameters are handled by SeparatedOutput

				if (polyColumns) {
					output.setParameter("PathMode", PolyPath::pathModeToString(mode));
				}

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
					//output.addHeading("trk", "deg");
					//output.addHeading("gs",  "knot");
					//output.addHeading("vs",  "fpm");
					output.addHeading("tcp_trk", "unitless");
					output.addHeading("tcp_gs", "unitless");
					output.addHeading("tcp_vs", "unitless");
					output.addHeading("radius", "NM");
					output.addHeading("accel_gs", "m/s^2");
					output.addHeading("accel_vs", "m/s^2");
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
					if (latlon) {
						output.addHeading("center_lat", "deg");
						output.addHeading("center_lon", "deg");
						output.addHeading("center_alt",  "ft");
					} else {
						output.addHeading("center_x",   "NM");
						output.addHeading("center_y",   "NM");
						output.addHeading("center_z",   "ft");
					}
					output.addHeading("info", "unitless");
				}
				output.addHeading("label", "unitless");
				if (latlon) {
					output.addHeading("alt2", "ft");//21
				} else {
					output.addHeading("sz2", "ft");//21
				}
				if (mode == PolyPath::USER_VEL || mode == PolyPath::USER_VEL_FINITE) {
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
				first_line = false;
			}

			PolyPath pp = p;

			if (mode != pp.getPathMode()) {
				error.addWarning("Attempting to convert PolyPath "+pp.getName()+" to user-specified mode "+PolyPath::pathModeToString(mode));
				pp.setPathMode(mode);
			}
			for (int i = 0; i < pp.size(); i++) {
				for (int j = 0; j < pp.getPolyRef(i).size(); j++) {
					output.addColumn(pp.toStringList(i,j,precision,tcpColumns));
					lines++;
					output.writeLine();
				}
			}
		} else {
			error.addWarning("Attempted to write polygons when setPolyPathMode() has not been called");
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
