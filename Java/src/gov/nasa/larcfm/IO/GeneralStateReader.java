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

package gov.nasa.larcfm.IO;

import gov.nasa.larcfm.Util.Constants;
import gov.nasa.larcfm.Util.ErrorLog;
import gov.nasa.larcfm.Util.ErrorReporter;
import gov.nasa.larcfm.Util.GeneralState;
import gov.nasa.larcfm.Util.LatLonAlt;
import gov.nasa.larcfm.Util.ParameterData;
import gov.nasa.larcfm.Util.ParameterProvider;
import gov.nasa.larcfm.Util.ParameterReader;
import gov.nasa.larcfm.Util.Point;
import gov.nasa.larcfm.Util.Position;
import gov.nasa.larcfm.Util.SimpleMovingPoly;
import gov.nasa.larcfm.Util.Util;
import gov.nasa.larcfm.Util.Velocity;
import gov.nasa.larcfm.Util.f;

import java.util.ArrayList;
import java.util.List;
import java.io.Reader;
import java.io.FileReader;
import java.io.IOException;
import java.io.FileNotFoundException;

/**
 * This reads in and stores a set of aircraft and polygon states, possibly over time, (and
 * parameters) from a file The aircraft states are stored in an
 * ArrayList&lt;GeneralState&gt;. A state file consists of comma or
 * space-separated values, with one position and velocity per line. Required
 * columns include aircraft name, 3 position columns (either x[NM]/y[NM]/z[ft]
 * or latitude[deg]/longitude[deg]/altitude[ft]) and 3 velocity columns (either
 * vx[kn]/vy[kn]/vz[fpm] or track[deg]/gs[kn]/vs[fpm]).
 * All empty lines or comment lines (starting with a hash sign (#)) are ignored.
 * Polygon information is one vertex per line, with an additional "top" column to
 * denote the polygon's upper altitude.
 * <p>
 *
 * While this reader allows for a time column, only the last
 * entry for an aircraft or polygon will be stored.  The GeneralSequenceReader is 
 * needed to allow for multiple state-time data points. 
 * If consecutive position and velocity lines are for the
 * same aircraft, subsequent name fields may be replaced with a double quotation
 * mark (&quot). The aircraft name is case sensitive, so US54A != Us54a !=
 * us54a.
 * <p>
 *
 * It is necessary to include a header line that defines the column ordering.
 * The column definitions are not case sensitive. There is also an optional
 * header line, immediately following the column definition, that defines the
 * unit type for each column (the defaults are listed above).
 * <p>
 *
 * Files may also include parameter definitions prior to other data. Parameter
 * definitions are of the form &lt;key&gt; = &lt;value&gt;, one per line, where
 * &lt;key&gt; is a case-insensitive alphanumeric word and &lt;value&gt; is
 * either a numeral or string. The &lt;value&gt; may include a unit, such as
 * "dist = 50 [m]". Note that parameters require a space on either side of the
 * equals sign. Note that it is possible to also update the stored parameter
 * values (or store additional ones) through API calls.
 * Parameters can be interpreted as double values, strings, or Boolean values,
 * and the user is required to know which parameter is interpreted as which
 * type.  However, something reasonable will come out, for instance a double 
 * value read as a string will come out as the string representation of the value.
 * <p>
 *
 *
 */
public class GeneralStateReader implements ParameterProvider, ParameterReader, ErrorReporter {
	protected ErrorLog error;
	protected SeparatedInput input;
	protected ArrayList<GeneralState> states;
	protected boolean hasRead;
	protected boolean latlon;
	protected boolean trkgsvs;
	protected boolean clock;
	protected int[] head = {-1,-2,-3,-4,-5,-6,-7,-8,-9}; // array size of number of elements (vector in C++)
	// we store the heading indices in the following order:
	protected final int NAME = 0;
	protected final int LAT_SX = 1;
	protected final int LON_SY = 2;
	protected final int ALT_SZ = 3;
	protected final int TRK_VX = 4;
	protected final int GS_VY = 5;
	protected final int VS_VZ = 6;
	protected final int TM_CLK = 7;
	protected final int SZ2 = 8; // used for polygons!!!

	protected final int POLY = -1;
	protected final int UNKNOWN = 0;
	protected final int STATE = 1;


	protected String fname;
	protected FileReader fr;

	/** A new, empty StateReader. After you have a StateReader object then use the open() method. */
	public GeneralStateReader() {
		error = new ErrorLog("GeneralStateReader()");
		states = new ArrayList<GeneralState>(0);
		input = new SeparatedInput();
		input.setCaseSensitive(false);            // headers & parameters are lower case
		fname = "";
	}

	/** Read a new file into an existing StateReader.  Parameters are preserved if they are not specified in the file. */
	public void open(String filename) {
		if (filename == null || filename.equals("")) {
			error.addError("No file specified");
			return;
		}
		close();
		fname = filename;
		try {
			fr = new FileReader(filename);
			open(fr);
		} catch (FileNotFoundException e) {
			error.addError("File "+filename+" read protected or not found");
			if (states != null) states.clear();
			return;
		}
		close();
	}

	/** Read a new file into an existing StateReader.  Parameters are preserved if they are not specified in the file. */
	public void open(Reader r) {
		if (r == null) {
			error.addError("Null Reader provided to GeneralStateReader");
			return;
		}
		fname = "<none>";
		SeparatedInput si;
		si = new SeparatedInput(r);
		si.setCaseSensitive(false);            // headers & parameters are lower case
		List<String> params = input.getParametersRef().getList();
		for (String p: params) {
			si.getParametersRef().set(p, input.getParametersRef().getString(p));
		}
		for (int i = 0; i < head.length; i++) {
			head[i] = -1;
		}
		input = si;
		loadfile();
	}

	private void close() {
		if (fr != null) {
			try {
				fr.close();
			} catch (IOException e) {
				error.addError("IO Exception in close(): "+e.getMessage());
			}
			fr = null;
		}
	}

	private void loadfile() {
		hasRead = false;
		clock = true;
		//interpretUnits = false;
		states = new ArrayList<GeneralState>(10);
		String name = ""; // the current aircraft name
		double lastTime = -1000000; // time must be increasing
		int stateIndex = -1;
		List<String> containmentList = new ArrayList<String>();

		// save accuracy info in temp vars
		double h = Constants.get_horizontal_accuracy();
		double v = Constants.get_vertical_accuracy();
		double t = Constants.get_time_accuracy();


		while ( ! input.readLine()) {
			// look for each possible heading
			if ( ! hasRead) {

				// process heading
				latlon = (altHeadings("lat", "lon", "long", "latitude") >= 0);
				clock = (altHeadings("clock", "") >= 0);
				trkgsvs = (altHeadings("trk", "track") >= 0);

				head[NAME]   = altHeadings("name", "aircraft", "id");
				head[LAT_SX] = altHeadings("sx", "lat", "latitude");
				head[LON_SY] = altHeadings("sy", "lon", "long", "longitude");
				head[ALT_SZ] = altHeadings5("sz", "alt", "altitude", "sz1", "alt1");
				head[SZ2]    = altHeadings("sz2", "alt2", "altitude_top", "poly_top");
				head[TRK_VX] = altHeadings("trk", "vx", "track");
				head[GS_VY]  = altHeadings("gs", "vy", "groundspeed", "groundspd");
				head[VS_VZ]  = altHeadings("vs", "vz", "verticalspeed", "hdot");
				head[TM_CLK] = altHeadings("clock", "time", "tm", "st");

				// set accuracy parameters
				if (getParametersRef().contains("horizontalAccuracy")) {
					Constants.set_horizontal_accuracy(getParametersRef().getValue("horizontalAccuracy","m"));
				}
				if (getParametersRef().contains("verticalAccuracy")) {
					Constants.set_vertical_accuracy(getParametersRef().getValue("verticalAccuracy","m"));
				}
				if (getParametersRef().contains("timeAccuracy")) {
					Constants.set_time_accuracy(getParametersRef().getValue("timeAccuracy","s"));
				}

				if (getParametersRef().contains("filetype")) {
					String sval = getParametersRef().getString("filetype");
					if (!sval.equalsIgnoreCase("state") && !sval.equalsIgnoreCase("generalstate")) {
						error.addError("Wrong filetype: "+sval);
						break;
					}
				}

				if (this.getParametersRef().contains("containment")) {
					containmentList = getParametersRef().getListString("containment");
				}

				hasRead = true;
				for (int i = 0; i <= VS_VZ; i++) {
					if (head[i] < 0) error.addError("At least one required heading was missing (look for [sx|lat,sy|lon,sz|alt] [vx|trk,vy|gs,vz|vs])");
				}
			} // end ! hasRead

			// determine what type of data this is...
			int linetype = UNKNOWN;
			if (input.columnHasValue(head[NAME]) && 
					input.columnHasValue(head[LAT_SX]) && 
					input.columnHasValue(head[LON_SY]) &&
					input.columnHasValue(head[ALT_SZ]) &&
					input.columnHasValue(head[TRK_VX]) && 
					input.columnHasValue(head[GS_VY]) &&
					input.columnHasValue(head[VS_VZ]) &&
					input.columnHasValue(head[TM_CLK])) {
				linetype = STATE;
				if (input.columnHasValue(head[SZ2])) {
					linetype = POLY;
				}
			} else {
				error.addError("Invalid data line "+input.lineNumber());
			}


			String thisName = input.getColumnString(head[NAME]);
			if (thisName.equals("\"")) {
				thisName = name; 
			}

			Position ss;
			Velocity vv;
			double tm = 0.0;
			if (head[TM_CLK] >= 0) {
				tm = parseClockTime(input.getColumnString(head[TM_CLK]));
				if (lastTime > tm && thisName.equals(name)) {
					error.addWarning("Time not increasing from "+f.Fm4(lastTime)+" to "+f.Fm4(tm)+" for aircraft "+name+", skipping non-consecutive data.");
					continue;
				}
			}

			name = thisName;

			stateIndex = getIndex(name);
			if ( stateIndex < 0) {
				stateIndex = size();
				if (linetype == POLY) {
					states.add(new GeneralState(name, new SimpleMovingPoly(), tm, containmentList.contains(thisName)));
				} else {
					states.add(new GeneralState(name, Position.INVALID, Velocity.INVALID, tm)); // need to replace later
				}
			}

			if (input.hasError()) {
				error.addError(input.getMessage());
				states.clear();
				break;
			}

			// the values are in the default units.
			if (latlon) {
				ss = new Position(LatLonAlt.mk(input.getColumn(head[LAT_SX], "deg"), 
						input.getColumn(head[LON_SY], "deg"), 
						input.getColumn(head[ALT_SZ], "ft")));
			} else {
				ss = new Position(Point.mk(
						input.getColumn(head[LAT_SX], "NM"), 
						input.getColumn(head[LON_SY], "NM"), 
						input.getColumn(head[ALT_SZ], "ft")));
			}

			if (trkgsvs) {
				vv = Velocity.mkTrkGsVs(
						input.getColumn(head[TRK_VX], "deg"), 
						input.getColumn(head[GS_VY], "knot"), 
						input.getColumn(head[VS_VZ], "fpm"));
			} else {
				vv = Velocity.mkVxyz(
						input.getColumn(head[TRK_VX], "knot"), 
						input.getColumn(head[GS_VY],  "knot"), 
						input.getColumn(head[VS_VZ],  "fpm"));
			}

			if (linetype == POLY) {
				double top = input.getColumn(head[SZ2], "ft");
				SimpleMovingPoly p = states.get(stateIndex).getPolygon();
				// replace existing entry if time does not match
				if (states.get(stateIndex).getTime() != tm) {
					p = new SimpleMovingPoly();
					states.set(stateIndex, new GeneralState(name,p,tm,containmentList.contains(thisName)));
				}
				p.setTop(top);
				p.addVertex(ss, vv);
			} else {
				// replace existing entry
				states.set(stateIndex, new GeneralState(name, ss, vv, tm));
			}

			lastTime = tm;


		}

		// reset accuracy parameters to their previous values
		Constants.set_horizontal_accuracy(h);
		Constants.set_vertical_accuracy(v);
		Constants.set_time_accuracy(t);

	}

	protected int altHeadings(String s1, String s2, String s3, String s4) {
		int r = input.findHeading(s1);
		if (r < 0 && !s2.equals("")) {
			r = input.findHeading(s2);
		}
		if (r < 0 && !s3.equals("")) {
			r = input.findHeading(s3);
		}
		if (r < 0 && !s4.equals("")) {
			r = input.findHeading(s4);
		}
		return r;
	}

	protected int altHeadings(String s1, String s2, String s3) {
		return altHeadings(s1,s2,s3,"");
	}

	protected int altHeadings(String s1, String s2) {
		return altHeadings(s1,s2,"","");
	}

	/**
	 * Return the string as a single value in seconds.   If the column is labeled "clock," then
	 * it is expected in a "HH:MM:SS" format.  If the column is labeled "time" then just read
	 * it as a value.  If the string cannot be parsed, return 0.0;
	 * @param s the string to be parsed
	 * @return
	 */
	protected double parseClockTime(String s) {
		double tm = 0.0;
		try {
			if (clock) {
				tm = Util.parse_time(s);
			} else {
				tm = input.getColumn(head[TM_CLK]); //getColumn(_sec, head[TM_CLK]);
			}
		} catch (NumberFormatException e) {
			error.addError("error parsing time at line "+input.lineNumber());
		}
		return tm;
	}

	protected int getIndex(String name) {
		for (int i=0; i < size(); i++) {
			GeneralState s = states.get(i);
			if (s.getName().equals(name)) {
				return i;
			}
		}
		return -1;
	}

	/** Return the number of AircraftStates in the file */
	public int size() {
		return states.size();
	}

	public int numAircraft() {
		int cnt = 0;
		for (int i = 0; i < states.size(); i++) {
			if (states.get(i).hasPointMass()) cnt++;
		}
		return cnt;
	}

	public int numPolygons() {
		int cnt = 0;
		for (int i = 0; i < states.size(); i++) {
			if (states.get(i).hasPolygon()) cnt++;
		}
		return cnt;
	}

	/** Returns the i-th aircraft state in the file.  This will be a deep copy. */
	public GeneralState getGeneralState(int ac) {
		if (ac < 0 || ac >= size()) return new GeneralState();
		return states.get(ac).copy();
	}

	/** Returns a (deep) copy of all AircraftStates in the file */
	public ArrayList<GeneralState> getGeneralStateList() {
		ArrayList<GeneralState> s = new ArrayList<GeneralState>(states.size());
		for (int i = 0; i < states.size(); i++) {
			s.add(states.get(i).copy());
		}
		return s;
	}

	public boolean isLatLon() {
		return latlon;
	}

	public String getFilename() {
		return fname;
	}

	protected int altHeadings5(String s1, String s2, String s3, String s4, String s5) {
		int r = input.findHeading(s1);
		if (r < 0 && !s2.equals("")) {
			r = input.findHeading(s2);
		}
		if (r < 0 && !s3.equals("")) {
			r = input.findHeading(s3);
		}
		if (r < 0 && !s4.equals("")) {
			r = input.findHeading(s4);
		}
		if (r < 0 && !s5.equals("")) {
			r = input.findHeading(s5);
		}
		return r;
	}

	//
	// ParameterReader methods
	//

	public ParameterData getParameters() {
		return new ParameterData(input.getParametersRef());
	}

	public ParameterData getParametersRef() {
		return input.getParametersRef();
	}

	public void updateParameterData(ParameterData p) {
		p.copy(input.getParametersRef(),true);
	}

	// ErrorReporter Interface Methods

	public boolean hasError() {
		return error.hasError() || input.hasError();
	}
	public boolean hasMessage() {
		return error.hasMessage() || input.hasMessage();
	}
	public String getMessage() {
		return error.getMessage() + input.getMessage();
	}
	public String getMessageNoClear() {
		return error.getMessageNoClear() + input.getMessageNoClear();
	}

	public String toString() {
		//return input.toString();
		String rtn = "StateReader: ------------------------------------------------\n";
		for (int j = 0; j < states.size(); j++) {
			rtn = rtn + states.get(j)+ "\n";
		}
		return rtn;
	}

}
