/* 
 * StateWriter
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
import gov.nasa.larcfm.Util.NavPoint;
import gov.nasa.larcfm.Util.Pair;
import gov.nasa.larcfm.Util.ParameterData;
import gov.nasa.larcfm.Util.Position;
import gov.nasa.larcfm.Util.Velocity;
import gov.nasa.larcfm.Util.f;

import java.io.Writer;
import java.io.FileWriter;
import java.io.BufferedWriter;
import java.io.IOException;
import java.io.Closeable;

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
final public class StateWriter implements ErrorReporter, Closeable {
	private ErrorLog error;
	private SeparatedOutput output;
	private boolean velocity;
	private boolean latlon;
	private boolean trkgsvs;
	private boolean display_time;
	private boolean first_line;
	private boolean display_units;
	private int precision;
	private int lines;
	private String fname;
	private final double default_time = 0.0;
	private Writer fw;
    
    /** A new StateWriter. */
	public StateWriter() {
		error = new ErrorLog("StateWriter");
        trkgsvs = true;
        velocity = true;
        display_time = true;
        display_units = true;
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
		error = new ErrorLog("StateWriter(Writer)");
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
	
	public boolean isVelocity() {
		return velocity;
	}

	public void setVelocity(boolean velocity) {
		this.velocity = velocity;
	}

	public boolean isTrkGsVs() {
		return trkgsvs;
	}

	public void setTrkGsVs(boolean trkgsvs) {
		this.trkgsvs = trkgsvs;
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

	public void writeState(String name, double time, Position p, Velocity v, String label) {
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
			output.addHeading("label", "");
			first_line = false;
		}
		output.addColumn(name);
		if (display_time) {
			output.addColumn(f.FmPrecision(time,precision));			
		}
		output.addColumn(p.toStringList(precision));
		if (velocity) {
			if (trkgsvs) {
				output.addColumn(v.toStringList(precision));							
			} else {
				output.addColumn(v.toStringXYZList(precision));											
			}
		}
		if (!label.equals("")) output.addColumn(label);
		lines++;
		output.writeLine();		
	}	
	
	public void writeState(String name, double time, Position p, Velocity v) {
		writeState(name,time,p,v,"");
	}

	public void writeState(String name, double time, Position p, String label) {
		writeState(name,time,p,Velocity.ZERO, label);
	}
	
	public void writeState(String name, double time, Position p) {
		writeState(name,time,p,Velocity.ZERO, "");
	}

	
	public void writeState(String name, double time, Pair<Position,Velocity> pv, String label) {
		writeState(name, time, pv.first, pv.second, label);
	}
	
	public void writeState(String name, Position p, Velocity v, String label) {
		writeState(name, default_time, p, v, label);
	}
	
	public void writeState(String name, Position p, Velocity v) {
		writeState(name, default_time, p, v, "");
	}

	
	public void writeState(String name, Position p, String label) {
		writeState(name, default_time, p, label);
	}
	
	public void writeState(String name, NavPoint np) {
		writeState(name,np.time(),np.position(),Velocity.ZERO,np.label());
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

    public String toString() {
    	String rtn = "StateWriter: ------------------------------------------------\n";
//    	for (int j = 0; j < states.size(); j++) {
//            rtn = rtn + states.get(j)+ "\n";
//    	}
    	return rtn;
    }

}
