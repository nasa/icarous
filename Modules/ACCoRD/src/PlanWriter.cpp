/* 
 * PlanWriter
 *
 * Contact: Jeff Maddalon
 * 
 * Copyright (c) 2011-2019 United States Government as represented by
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
        precision = 8;
        latLonExtraPrecision = 2;
        mode = PolyPath::MORPHING;
        polyColumns = false;
        trkgsvs = true;
//        timeOffset = 0.0;
        lines = 0;
        first_line = true;
        latlon = true;
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

	int PlanWriter::getLatLonExtraPrecision() const {
		return latLonExtraPrecision;
	}

	void PlanWriter::setLatLonExtraPrecision(int prec) {
		latLonExtraPrecision = prec;
	}

//	double PlanWriter::getTimeOffset() const {
//		return timeOffset;
//	}
//
//	void PlanWriter::setTimeOffset(double offset) {
//		if (offset >= 0.0) {
//			timeOffset = offset;
//		}
//	}


	void PlanWriter::setPolyPathMode(PolyPath::PathMode m) {
		polyColumns = true;
		mode = m;
	}

	void PlanWriter::setPolygons(bool b) {
		polyColumns = b;
	}


	void PlanWriter::clearPolyPathMode() {
		polyColumns = false;
		mode = PolyPath::MORPHING;
	}

	void PlanWriter::setContainmentParameter(const std::string& s) {
		output.setParameter("containment", s);
	}

	void PlanWriter::setPolyPathParameters(const std::vector<PolyPath>& list) {
		std::vector<PolyPath> pplist;
		if (list.size() > 0) {
			bool found = false;
			std::string s = "";
			for (int i = 0; i < (int) list.size(); i++) {
					if (!found) {
						setPolygons(true);
						setPolyPathMode(list[i].getPathMode());
					}
					if (list[i].isContainment()) {
						pplist.push_back(list[i]);
					}
					std::string note = list[i].getNote();
					if (!equals(note,"")) {
						output.setParameter(list[i].getID()+"_note", note);
					}
			}
			if (pplist.size() > 0) {
				s = pplist[0].getID();
				for (int i = 1; i < (int) pplist.size(); i++) {
					s = s+","+pplist[i].getID();
				}
			}
			if (pplist.size() > 0) {
				setContainmentParameter(s);
			}
		}
	}

	void PlanWriter::setPlanParameters(const std::vector<Plan>& plans) {
		if (plans.size() > 0) {
			latlon = plans[0].isLatLon();
			for (int i = 0; i < (int) plans.size(); i++) {
				std::string note = plans[i].getNote();
				if (!equals(note,"")) {
					output.setParameter(plans[i].getID()+"_note", note);
				}
			}
		}
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


//	void PlanWriter::writeLn(const std::string& str) {
//		output.writeLine(str);
//	}

	void PlanWriter::writeHeader(bool write_tcp, bool ll) {
		if (first_line) {
			tcpColumns = write_tcp;
			latlon = ll;
			output.setOutputUnits(display_units);

			// Comments and parameters are handled by SeparatedOutput

			if (polyColumns) {
				output.setParameter("PathMode", PolyPath::pathModeToString(mode));
			}

//			if (timeOffset > 0.0) {
//				output.setParameter("timeOffset", Fm8(timeOffset));
//
//			}

			output.addHeading("ID", "unitless");
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
//				if (latlon) {
//					output.addHeading("src_lat", "deg");
//					output.addHeading("src_lon", "deg");
//					output.addHeading("src_alt",  "ft");
//				} else {
//					output.addHeading("src_x",   "NM");
//					output.addHeading("src_y",   "NM");
//					output.addHeading("src_z",   "ft");
//				}
//				output.addHeading("src_time", "s");
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
			output.addHeading("name", "unitless");
			if (polyColumns) {
				if (latlon) {
					output.addHeading("alt2", "ft");//21
				} else {
					output.addHeading("sz2", "ft");//21
				}
				if (mode == PolyPath::USER_VEL || mode == PolyPath::USER_VEL_FINITE || mode == PolyPath::USER_VEL_EVER) {
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

	}

	void PlanWriter::writePlan(const Plan& p, bool write_tcp) {
		//fpln(" $$$$$$$$ writePlan: first_line = "+bool2str(first_line));

		Plan pp = p;

		writeHeader(write_tcp, p.isLatLon());

//		if (timeOffset > 0 && timeOffset <= p.getFirstTime()) {
//			pp = Plan(p);
//			pp.timeShiftPlan(0,-timeOffset);
//		} else if (timeOffset < 0 || timeOffset > p.getFirstTime()) {
//			error.addError("Invalid time offset: "+timeOffset+" plan first time="+p.getFirstTime());
//			fpln("PlanWriter ERROR: invalid time offset");
//		}

		for (int i = 0; i < pp.size(); i++) {
			output.addColumn(pp.toStringList(i,precision,tcpColumns));
			if (polyColumns) {
				output.addColumn("-"); //alt2
				if (mode == PolyPath::USER_VEL || mode == PolyPath::USER_VEL_FINITE || mode == PolyPath::USER_VEL_EVER) {
					output.addColumn("-"); //velocity
					output.addColumn("-"); //velocty
					output.addColumn("-"); //velocity
				}
			}
			lines++;
			output.writeLine();
		}
		output.flush();
	}	
	
	void PlanWriter::writePolyPath(const PolyPath& p, bool write_tcp) {
		//fpln(" $$$ writePolyPath: ENTER: p.size() = "+Fm0(p.size())+" "+bool2str(first_line)+" "+bool2str(polyColumns));
		//fpln(" $$$ writePolyPath: ENTER p.getPolyRef(0).size() = "+Fm0(p.getPoly(0).size()));
		if (polyColumns) {

			writeHeader(write_tcp, p.isLatLon());

			PolyPath pp = p;

//			if (timeOffset <= pp.getFirstTime()) {
//				pp.timeshift(-timeOffset);
//			} else if (timeOffset < 0 || timeOffset > p.getFirstTime()) {
//				error.addError("Invalid time offset: "+timeOffset+" polypath first time="+p.getFirstTime());
//				fpln("PlanWriter ERROR: invalid time offset");
//			}

			if (mode != pp.getPathMode()) {
				error.addWarning("Attempting to convert PolyPath "+pp.getID()+" to user-specified mode "+PolyPath::pathModeToString(mode));
				pp.setPathMode(mode);
			}
			for (int i = 0; i < pp.size(); i++) {
				//fpln(" $$$ writePolyPath: pp.getPolyRef("+Fm0(i)+").size() = "+Fm0(pp.getPolyRef(i).size()));
				for (int j = 0; j < pp.getPolyRef(i).size(); j++) {   // TODO: RWB IS THE Ref ok?
					if (pp.getPolyRef(i).size() < 3) {
						error.addWarning("Polygon "+pp.getID()+"("+Fm0(i)+") has fewer than 3 sides!");
					}
					output.addColumn(pp.toStringList(i,j,precision,latLonExtraPrecision,tcpColumns));
					lines++;
					output.writeLine();
				}
			}
		} else {
			error.addWarning("Attempted to write polygons when setPolyPathMode() has not been called");
		}
		output.flush();
	}

	int PlanWriter::size() const {
		return lines;
	}

    bool PlanWriter::isLatLon() const {
    	return latlon;
    }

    void PlanWriter::flush() {
    	output.flush();
    }

	void PlanWriter::write(const std::string& filename, const std::vector<Plan>& planlist, const std::vector<PolyPath>& polylist, bool write_tcp) {
		PlanWriter pw;
		pw.open(filename);
		if (!pw.hasError()) {
			// process plans and polypaths for extra parameters
			pw.setPlanParameters(planlist);
			pw.setPolyPathParameters(polylist);
			for (int i = 0; i < (int) planlist.size(); i++) {
				pw.writePlan(planlist[i], write_tcp);
			}
			for (int i = 0; i < (int) polylist.size(); i++) {
				pw.writePolyPath(polylist[i], write_tcp);
			}
			pw.flush();
		}
		if (pw.hasError()) {
			std::cout << "PlanWriter.write error: " << pw.getMessage() << std::endl;
		}
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
