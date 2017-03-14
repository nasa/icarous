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

package gov.nasa.larcfm.IO;

import gov.nasa.larcfm.Util.ErrorLog;
import gov.nasa.larcfm.Util.ErrorReporter;
import gov.nasa.larcfm.Util.ParameterData;
import gov.nasa.larcfm.Util.Plan;
import gov.nasa.larcfm.Util.PolyPath;
import gov.nasa.larcfm.Util.f;
import gov.nasa.larcfm.Util.PolyPath.PathMode;

import java.io.Writer;
import java.io.FileWriter;
import java.io.BufferedWriter;
import java.io.IOException;
import java.io.Closeable;

public class PlanWriter implements ErrorReporter, Closeable {
	private ErrorLog error;
	private SeparatedOutput output;
	private boolean latlon;
	private boolean first_line;
	private boolean display_units;
	private boolean tcpColumns;
	private boolean polyColumns;
	private boolean trkgsvs;
	private PolyPath.PathMode mode;
	private int precision;
	private int lines;
	private String fname;
	private Writer fw;

	/** A new PlanWriter. */
	public PlanWriter() {
		error = new ErrorLog("PlanWriter");
		display_units = true;
		tcpColumns = false;
		polyColumns = false;
		mode = PolyPath.PathMode.MORPHING;
		trkgsvs = true;
		precision = 6;
	}

	/** A new StateReader based on the given file. */
	public void open(String filename) {
		fname = filename;
		if (filename == null || filename.equals("")) {
			error.addError("No file specified");
			return;
		}
		close();
		try { 
			fw = new BufferedWriter(new FileWriter(filename));
		} catch (IOException e) {
			error.addError("File "+fname+" read protected or not found");
			return;
		}
		open(fw);
	}

	public void open(Writer writer) {
		error = new ErrorLog("PlanWriter(Writer)");
		if (writer == null) {
			error.addError("Null supplied for Writer in open()");
			return;
		}
		fw = writer;
		output = new SeparatedOutput(writer);
		first_line = true;
		lines = 0;
	}

	public void close() {
		if (fw != null) {
			try {
				//output.close();
				fw.close();
			} catch (IOException e) {
				error.addError("Exception on close(): "+e.getMessage());
			}
			fw = null;
		}
	}

	public int getPrecision() {
		return precision;
	}

	public void setPrecision(int precision) {
		this.precision = precision;
	}


	/**
	 * Indicate polygons will be output and set the path mode to be output.  This must be called before the first write command,
	 * otherwise any attempts to write a polygon will result in a warning and no polygon written.
	 * The default mode is PolyPath.PathMode.MORPHING.
	 * @param m mode to write polygons.
	 */
	public void setPolyPathMode(PolyPath.PathMode m) {
		mode = m;
		polyColumns = true;
	}

	/**
	 * Indicate that no polygons will be output (the default).
	 */
	public void clearPolyPathMode() {
		mode = PolyPath.PathMode.MORPHING;
		polyColumns = false;		
	}

	/** 
	 * Sets the column delimiter to a tab.  This method can only be used before the first "writeState" method.
	 */
	public void setColumnDelimiterTab() {
		if (first_line) {
			output.setColumnDelimiterTab();
		}
	}

	/** 
	 * Sets the column delimiter to a comma.  This method can only be used before the first "writeState" method.
	 */
	public void setColumnDelimiterComma() {
		if (first_line) {
			output.setColumnDelimiterComma();
		}
	}

	/** 
	 * Sets the column delimiter to a space.  This method can only be used before the first "writeState" method.
	 */
	public void setColumnDelimiterSpace() {
		if (first_line) {
			output.setColumnDelimiterSpace();
			output.setEmptyValue("0");
		}
	}

	/** 
	 * Adds a comment line to the file.
	 * @param comment comment string
	 */
	public void addComment(String comment) {
		output.addComment(comment);
	}

	/** 
	 * Set parameters.  Use all the parameters in the reader.
	 * @param pr parameters
	 */
	public void setParameters(ParameterData pr) {
		output.setParameters(pr);
	}

	public void clearParameters() {
		output.clearParameters();
	}

	public void writeLn(String str) {
		output.write(str+"\n");
	}

	/**
	 * Write a plan.
	 * NB: Any Plan.note fields must be included in the parameters before this call (if relevant). 
	 * @param p
	 * @param write_tcp
	 */
	public void writePlan(Plan p, boolean write_tcp) {
		//f.pln(" $$$ writePlan: p.size() = "+p.size());
		if (first_line) {
			tcpColumns = write_tcp;
			latlon = p.isLatLon();
			output.setOutputUnits(display_units);

			// Comments and parameters are handled by SeparatedOutput

			if (polyColumns) {
				output.setParameter("PathMode", mode.toString());
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
				if (mode == PathMode.USER_VEL || mode == PathMode.USER_VEL_FINITE) {				
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
				if (mode == PathMode.USER_VEL || mode == PathMode.USER_VEL_FINITE) {				
					output.addColumn("-"); //velocity
					output.addColumn("-"); //velocty
					output.addColumn("-"); //velocity
				}
			}
			lines++;
			output.writeLine();
		}
	}	

	/**
	 * Write a PolyPath.  setPolyPathMode() must be called prior to the first call to this.
	 * @param p
	 * @param write_tcp
	 */
	public void writePolyPath(PolyPath p, boolean write_tcp) {
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
					output.setParameter("PathMode", mode.toString());
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
				if (mode == PathMode.USER_VEL || mode == PathMode.USER_VEL_FINITE) {				
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

			if (mode != p.getPathMode()) {
				error.addWarning("Attempting to convert PolyPath "+p.getName()+" to user-specified mode "+mode.toString());
				p.setPathMode(mode);
			}
			for (int i = 0; i < p.size(); i++) {
				for (int j = 0; j < p.getPolyRef(i).size(); j++) {
					output.addColumn(p.toStringList(i,j,precision,tcpColumns));
					lines++;
					output.writeLine();
				}
			}
		} else {
			error.addWarning("Attempted to write polygons when setPolyPathMode() has not been called");
		}
	}	

	/** Return the number of lines added to the file */
	public int size() {
		return lines;
	}

	public boolean isLatLon() {
		return latlon;
	}


	// ErrorReporter Interface Methods

	public boolean hasError() {
		return error.hasError() || output.hasError();
	}
	public boolean hasMessage() {
		return error.hasMessage() || output.hasMessage();
	}
	public String getMessage() {
		return error.getMessage() + output.getMessage();
	}
	public String getMessageNoClear() {
		return error.getMessageNoClear() + output.getMessageNoClear();
	}

}
