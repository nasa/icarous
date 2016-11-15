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

package gov.nasa.larcfm.IO;

import gov.nasa.larcfm.Util.Constants;
import gov.nasa.larcfm.Util.ErrorLog;
import gov.nasa.larcfm.Util.ErrorReporter;
import gov.nasa.larcfm.Util.GeneralPlan;
import gov.nasa.larcfm.Util.ParameterData;
import gov.nasa.larcfm.Util.Plan;
import gov.nasa.larcfm.Util.PolyPath;
import gov.nasa.larcfm.Util.f;

import java.io.Writer;
import java.io.FileWriter;
import java.io.BufferedWriter;
import java.io.IOException;
import java.io.Closeable;
import java.util.ArrayList;
import java.util.List;
import java.util.stream.Collectors;

/**
 * This object writes a set of aircraft states, possibly over time, (and parameters) from a file
 * The Aircraft states are stored in an ArrayList<AircraftState>.
 *
 * State files consist of comma or space-separated values, with one point per line.
 * Required columns include aircraft name, 3 position columns (either x[nmi]/y[nmi]/z[ft] or 
 * latitude[deg]/longitude[deg]/altitude[ft]) and
 * 3 velocity columns (either vx[knot]/vy[knot]/vz[fpm] or track[deg]/gs[knot]/vs[fpm]).
 *
 * An optional column is time [s].  If it is included, a "history" will be build if an aircraft has more than one entry.
 * If it is not included, only the last entry for an aircraft will be stored.
 *
 * It is necessary to include a header line that defines the column ordering.  The column definitions are not case sensitive.
 * There is also an optional header line, immediately following the column definition, that defines the unit type for each
 * column (the defaults are listed above).
 *
 * If points are consecutive for the same aircraft, subsequent name fields may be replaced with a double quotation mark (&quot).
 * The aircraft name is case sensitive, so US54A != Us54a != us54a.
 *
 * Any empty line or any line starting with a hash sign (#) is ignored.
 *
 * Files may also include parameter definitions prior to other data.  Parameter definitions are of the form &lt;key&gt; = &lt;value&gt;,
 * one per line, where &lt;key&gt; is a case-insensitive alphanumeric word and &lt;value&gt; is either a numeral or string.  The &lt;value&gt;
 * may include a unit, such as "dist = 50 [m]".  Note that parameters require a space on either side of the equals sign.
 * Note that it is possible to also update the stored parameter values (or store additional ones) through API calls.
 *
 * Parameters can be interpreted as double values, strings, or Boolean values, and the user is required to know which parameter is
 * interpreted as which type.
 *
 * If the optional parameter "filetype" is specified, its value must be "state" or "history" (no quotes) for this reader to accept the 
 * file without error.
 *
 */
final public class GeneralPlanWriter implements ErrorReporter, Closeable {
	private ErrorLog error;
	private SeparatedOutput output;
	private boolean velocity;
	private boolean latlon;
	private boolean trkgsvs;
	private boolean display_time;
	private boolean first_line;
	private boolean display_units;
	private boolean polygons; // includes polygons (needs extra header)
	private boolean source;
	private boolean time2;
	private int precision;
	private int lines;
	private int num;
	private String fname;
	private final double default_time = 0.0;
	private Writer fw;

	
	protected final int NAME = 0;
	protected final int LAT_SX = 1;
	protected final int LON_SY = 2;
	protected final int SZ = 3;
	protected final int TIME = 4; // or clock
	protected final int LABEL = 5; // optional
	protected final int TYPE = 6;	// optional
	protected final int TCP_TRK = 7;
	protected final int TCP_GS = 8;
	protected final int TCP_VS = 9;
	protected final int ACC_TRK = 10;
	protected final int ACC_GS = 11;
	protected final int ACC_VS = 12;
	protected final int TRK = 13;
	protected final int GS = 14;
	protected final int VS = 15;
	protected final int SRC_LAT_SX = 16;
	protected final int SRC_LON_SY = 17;
	protected final int SRC_ALT = 18;
	protected final int SRC_TIME = 19;
	protected final int SZ2 = 20;
	protected final int TIME2 = 21; // this is an "activation time" after which the plan goes live
	protected final int RADIUS = 22;

	
	/** A new GeneralStateWriter. */
	public GeneralPlanWriter() {
		error = new ErrorLog("GeneralPlanWriter");
		trkgsvs = true;
		velocity = true;
		display_time = true;
		display_units = true;
		polygons = true;
		source = true;
		time2 = true;
		precision = 6;
	}

	/** A new GeneralStateWriter based on the given file. */
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
			error.addError("File "+fname+" write protected or not found");
			return;
		}
		open(fw);
	}

	public void open(Writer writer) {
		error = new ErrorLog("GeneralStateWriter(Writer)");
		if (writer == null) {
			error.addError("Null supplied for Writer in open()");
			return;
		}
		fw = writer;
		output = new SeparatedOutput(writer);
		first_line = true;
		lines = 0;
		num = 0;
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


	public boolean isTrkGsVs() {
		return trkgsvs;
	}

	public void setTrkGsVs(boolean trkgsvs) {
		this.trkgsvs = trkgsvs;
	}

	public boolean hasPolygons() {
		return polygons;
	}

	/**
	 * Toggle extra polygon headers, true=include (default true)
	 * @param p
	 */
	public void setPolygons(boolean p) {
		polygons = p;
	}


	public int getPrecision() {
		return precision;
	}

	public void setPrecision(int precision) {
		this.precision = precision;
	}

	/** Will the time be added to the file */
	public boolean isOutputTime() {
		return display_time;
	}

	/** Should the time be added to the file */
	public void setOutputTime(boolean display_time) {
		this.display_time = display_time;
	}

	/** Will the units be displayed? */
	public boolean isOutputUnits() {
		return display_units;
	}

	/** Should the units be displayed? */
	public void setOutputUnits(boolean display_units) {
		this.display_units = display_units;
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
	
	public void setParameterPathMode(PolyPath.PathMode mode) {
		output.setParameter("pathmode", mode.toString());
	}

	public void setParameterPathMode(PolyPath path) {
		output.setParameter("pathmode", path.getPathMode().toString());
	}

	public void setParameterPathMode(String s) {
		output.setParameter("pathmode", s);
	}

	public void setParameterHorizontalAccuracy(String unit, double d) {
		output.setParameter("horizontalAccuracy", d+"["+unit+"]");
	}

	public void setParameterVerticalAccuracy(String unit, double d) {
		output.setParameter("verticalAccuracy", d+"["+unit+"]");
	}

	public void setParameterContainment(List<GeneralPlan> list) {
		List<GeneralPlan> pplist = list.stream().filter(x -> x.isContainment()).collect(Collectors.toList());
		String s = "";
		if (pplist.size() > 0) {
			s = pplist.get(0).getName();
			for (int i = 1; i < pplist.size(); i++) {
				s = s+","+pplist.get(i).getName();
			}
		}
		output.setParameter("containment", s);
	}

	public void setParameterContainment(String s) {
		output.setParameter("containment", s);
	}

	public void writePlan(GeneralPlan gp, double activation_time) {
		String name = gp.getName();
		if (first_line) {
			latlon = gp.isLatLon();
			output.setOutputUnits(display_units);
			if (gp.hasPolyPath()) {
				polygons = true;
			}

			// Comments and parameters are handled by SeparatedOutput

		
			output.addHeading("name", "unitless");//0
			if (latlon) {
				output.addHeading("lat",  "deg");//1
				output.addHeading("lon",  "deg");//2
				output.addHeading("alt",  "ft");//3
			} else {
				output.addHeading("sx",   "NM");//1
				output.addHeading("sy",   "NM");//2
				output.addHeading("sz",   "ft");//3
			}
			if (display_time) {
				output.addHeading("time", "s");//4		
			}
			output.addHeading("type","unitless");//5
			if (velocity) {
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
			output.addHeading("tcp_trk","unitless");//9
			output.addHeading("accel_trk", "deg/s");//10
			output.addHeading("tcp_gs","unitless");//11
			output.addHeading("accel_gs", "m/s^2");//12
			output.addHeading("tcp_vs","unitless");//13
			output.addHeading("accel_vs", "m/s^2");//14
			output.addHeading("radius", "NM");
			if (source) {
				if (latlon) {
					output.addHeading("src_lat",  "deg");//15
					output.addHeading("src_lon",  "deg");//16
					output.addHeading("src_alt",  "ft");//17
				} else {
					output.addHeading("src_sx",   "NM");//15
					output.addHeading("src_sy",   "NM");//16
					output.addHeading("src_sz",   "ft");//17
				}
				output.addHeading("src_time", "s");//18
			}
			output.addHeading("label", "unitless");//19
			if (polygons) {
				output.addHeading("alt2", "ft");//20
			}
			if (time2) {
				output.addHeading("active_time", "s");//21
			}
			first_line = false;
		}

		if (gp.hasPolyPath()) {
			PolyPath pp = gp.getPolyPath();
			for (int i = 0; i < pp.size(); i++) {
				for (int j = 0; j < pp.getPolyRef(i).size(); j++) {
					output.addColumn(pp.toStringList(i, j, 6, true));
					output.addColumn(f.Fm6(activation_time));
					lines++;
					output.writeLine();
				}
			}
		} else {
			Plan p = gp.getPlan();
			for (int i = 0; i < p.size(); i++) {
				output.addColumn(p.toStringList(i, 6, true));
				output.addColumn("-"); // alt2
				output.addColumn(f.Fm6(activation_time));
				lines++;
				output.writeLine();
			}
		}
		num++;
	}	

	public void writePlan(GeneralPlan gp) {
		writePlan(gp,0.0);
	}

	public void writePlan(Plan p, double t) {
		writePlan(new GeneralPlan(p), t);
	}

	public void writePlan(PolyPath p, double t) {
		writePlan(new GeneralPlan(p), t);
	}

	public void writePlan(Plan p) {
		writePlan(new GeneralPlan(p));
	}

	public void writePlan(PolyPath p) {
		writePlan(new GeneralPlan(p));
	}


	/** Return the number of states added to the file */
	public int size() {
		return num;
	}

	/**
	 * Return number of lines added
	 */
	public int lines() {
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

	public String toString() {
		String rtn = "StateWriter: ------------------------------------------------\n";
		//    	for (int j = 0; j < states.size(); j++) {
		//            rtn = rtn + states.get(j)+ "\n";
		//    	}
		return rtn;
	}

}
