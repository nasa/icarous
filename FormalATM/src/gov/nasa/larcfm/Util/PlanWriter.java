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

package gov.nasa.larcfm.Util;

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
	private int precision;
	private int lines;
	private String fname;
	private Writer fw;

	/** A new PlanWriter. */
	public PlanWriter() {
		error = new ErrorLog("PlanWriter");
		display_units = true;
		tcpColumns = false;
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
				output.close();
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
	 */
	public void addComment(String comment) {
		output.addComment(comment);
	}

	/** 
	 * Set parameters.  Use all the parameters in the reader.
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
	
	public void writePlan(Plan p, boolean tcpColumnsLocal) {
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



	/** Return the number of states added to the file */
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
