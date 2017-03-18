/* 
 * PolyReader
 *
 * Note: if the accuracy parameters (horizontalAccuracy, verticalAccuracy, timeAccuracy) 
 * are changed in a file, ONLY THOSE will be interpreted here (and values will be re-set
 * afterwards). This is necessary to correctly read in a PolyPath.  They may need to be 
 * explicitly set later to do any manipulations of the PolyPaths.
 *
 * Contact: George Hagen
 *
 * Copyright (c) 2011-2017 United Starm test_r	tes Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 *  */

package gov.nasa.larcfm.IO;


import gov.nasa.larcfm.Util.Constants;
import gov.nasa.larcfm.Util.ErrorLog;
import gov.nasa.larcfm.Util.ErrorReporter;
import gov.nasa.larcfm.Util.KinematicsPosition;
import gov.nasa.larcfm.Util.NavPoint;
import gov.nasa.larcfm.Util.Pair;
import gov.nasa.larcfm.Util.ParameterData;
import gov.nasa.larcfm.Util.ParameterProvider;
import gov.nasa.larcfm.Util.Plan;
import gov.nasa.larcfm.Util.PlanUtil;
import gov.nasa.larcfm.Util.PolyPath;
import gov.nasa.larcfm.Util.Position;
import gov.nasa.larcfm.Util.TcpData;
import gov.nasa.larcfm.Util.Util;
import gov.nasa.larcfm.Util.Velocity;
import gov.nasa.larcfm.Util.f;

import java.io.*;
import java.util.ArrayList;
import java.util.List;

/**
 * This reads in and stores both Plans and PolyPaths (and parameters) from a file
 * 
 * Plan files consist of comma or space-separated values, with one point per line.
 * Required columns include either:
 * Aircraft name, x-position [nmi], y-position [nmi], z-position [ft], time [s]
 * or
 * Aircraft name, latitude [deg], longitude [deg], altitude [ft], time [s]
 * 
 * Polygon files also include a required second altitude column [ft] (e.g. alt2).
 * 
 * Any given line may contain either type of information, but all lines describing a single entity (either aircraft or polygon)
 * must be located together, and without time decreasing -- new entities are detected by a change in the name field or if
 * (time of entry n) &lt; (time of entry n+1).  The type of the entry is defined by the presence or lack of a second 
 * altitude column value (polys have it, plans don't).
 *
 * Optional columns include point mutability and point label, both with [unspecified] units.  Even if these columns are defined, 
 * these two values are optional for any given line.  Columns without values may be left blank (if comma-delimited) or indicated
 * by a single dash (-).
 *
 * It is necessary to include a header line that defines the column ordering.  The column definitions are not case sensitive.
 * There is also an optional header line, immediately following the column definition, that defines the unit type for each
 * column (the defaults are listed above).
 *
 * If points are consecutive for the same aircraft, subsequent name fields may be replaced with a double quotation mark (").
 * The aircraft name is case sensitive, so US54A != Us54a != us54a.
 *
 * Any empty line or any line starting with a hash sign (#) is ignored.
 *
 * Files may also include parameter definitions prior to other data.  Parameter definitions are of the form <code>key</code> = <code>value</code>,
 * one per line, where <code>key</code> is a case-insensitive alphanumeric word and <code>value</code> is either a numeral or string.  The <code>value</code> 
 * may include a unit, such as "dist = 50 [m]".  Note that parameters require a space on either side of the equals sign.
 *
 * Parameters can be interpreted as double values, strings, or Boolean values, and the user is required to know which parameter is
 * interpreted as which type.
 * 
 * If the optional parameter "filetype" is specified, its value must be "plan", "trajectory", "poly", or "plan+poly" (no quotes)
 * for this reader to accept the file without error.
 * 
 * Plan "note" information is to be stored with a parameter named ID_note, where ID is the name of the plan in question.
 * 
 */
public class PlanReader implements ParameterProvider, /*ParameterReader,*/ ErrorReporter { // extends PlanReader { 
	protected ErrorLog error;
	protected SeparatedInput input;
	protected ArrayList<Plan> plans = new ArrayList<Plan>();
	protected boolean clock;
	// we store the heading indices in the following order:
	protected final int NAME = 0; // not included in TCP_OUTPUT_COLUMNS
	protected final int LAT_SX = 1;
	protected final int LON_SY = 2;
	protected final int SZ = 3;
	protected final int TIME = 4;   // or clock
	protected final int TYPE = 5;	// optional
	protected final int TCP_TRK = 6;
	protected final int TCP_GS = 7;
	protected final int TCP_VS = 8;
	protected final int RADIUS = 9; 
	protected final int ACC_GS = 10;
	protected final int ACC_VS = 11;
	protected final int SRC_LAT_SX = 12; // will be deprecated
	protected final int SRC_LON_SY = 13; // will be deprecated
	protected final int SRC_ALT = 14; // will be deprecated
	protected final int SRC_TIME = 15; // may be deprecated
	protected final int CENTER_LAT_SX = 16; 
	protected final int CENTER_LON_SY = 17; 
	protected final int CENTER_ALT    = 18;
	protected final int INFO = 19; 
	protected final int LABEL = 20; 

	protected ArrayList<PolyPath> paths = new ArrayList<PolyPath>();
	protected ArrayList<PolyPath> containment = new ArrayList<PolyPath>();
	// we store the heading indices in the following order:
	protected final int SZ2 = TcpData.TCP_OUTPUT_COLUMNS+1; // polygons
	protected final int TRK = TcpData.TCP_OUTPUT_COLUMNS+2; //polygons
	protected final int GS = TcpData.TCP_OUTPUT_COLUMNS+3; //polygons
	protected final int VS = TcpData.TCP_OUTPUT_COLUMNS+4; // polygons
	protected final int ENDTIME = TcpData.TCP_OUTPUT_COLUMNS+5; // optional field for polygons
	protected final int ACC_TRK = TcpData.TCP_OUTPUT_COLUMNS+6; // depricated TCP data

	protected int[] head = new int[TcpData.TCP_OUTPUT_COLUMNS+7]; // array size of number of elements (vector in C++), +1 for name field

	// data line types
	private final int POLY = -1;
	private final int UNKNOWN = 0;
	private final int PLAN = 1;

	/** Create a new PolyReader reading the specified file. 
	 * @param filename file name
	 * */
	public PlanReader(String filename) {
		open(filename);
	}
	
	/** Create a new PolyReader. */
	public PlanReader() {
	}
	
	public void open(String filename) {
		error = new ErrorLog("PolyReader("+filename+")");		
		if (filename == null || filename.equals("")) {
			input = new SeparatedInput();
			error.addError("Empty file name.");
			return;
		}
		try {
			FileReader fr = new FileReader(filename);
			open(fr);
		} catch (FileNotFoundException e) {
			input = new SeparatedInput();
			error.addError("File "+filename+" read protected or not found");
			if (plans != null) plans.clear();
			if (paths != null) paths.clear();
			if (containment != null) containment.clear();
			return;
		}
	}

	
	public void open(Reader fr) {
		if (error == null) {
			error = new ErrorLog("PolyReader(no name)");		
		}
		
		SeparatedInput si;
		si = new SeparatedInput(fr);
		input = si;
		loadfile();
	}
	
	
	private void loadfile() {
		boolean hasRead = false;
		clock = true;
		boolean latlon = true;
		boolean tcpinfo = true;
		boolean trkgsvs = true;
		//interpretUnits = false;
		plans = new ArrayList<Plan>(10);
		paths = new ArrayList<PolyPath>(10);
		containment = new ArrayList<PolyPath>(0);
		String name = ""; // the current aircraft name
		int pathIndex = -1;
		int containmentIndex = -1;
		int planIndex = -1;
		List<String> containmentList = new ArrayList<String>();
		boolean containmentLine = false;
		//	    double lastTime = -1000000; // time must be increasing

		PolyPath.PathMode pathmode = PolyPath.PathMode.MORPHING; 
		//		boolean calcpolyvels = true;
		//		boolean morphpolys = true;

		input.setCaseSensitive(false);            // headers & parameters are lower case

		// save accuracy info in temp vars
		double h = Constants.get_horizontal_accuracy();
		double v = Constants.get_vertical_accuracy();
		double t = Constants.get_time_accuracy();

		//        boolean kinematic = false;
		//boolean convert = false;
		while ( ! input.readLine()) {
			// look for each possible heading
			if ( ! hasRead) {
				// process heading
				latlon = true;
				if (altHeadings("lat", "lon", "long", "latitude") < 0) {
					latlon = false;
				}
				if (altHeadings("trk", "v_trk", "track") < 0) {
					trkgsvs = false;
				}

				if (input.findHeading("clock") < 0) {
					clock = false;
				}

				head[NAME] =    altHeadings("name", "aircraft", "id");
				head[LAT_SX] =  altHeadings("sx", "lat", "latitude");
				head[LON_SY] =  altHeadings("sy", "lon", "long", "longitude");
				head[SZ] =      altHeadings5("sz", "alt", "altitude", "sz1", "alt1");
				head[SZ2] =     altHeadings("sz2", "alt2", "altitude_top", "poly_top");
				head[TIME] =    altHeadings("clock", "time", "tm", "st");
				//				head[MUTABLE] = altHeadings("mutable", "mutability");
				head[LABEL] =   altHeadings("label","point_name", "fix");
				head[INFO] =   altHeadings("info", "information", "tcp_info");

				head[TYPE] = 	input.findHeading("type");
				head[TCP_TRK] =	input.findHeading("tcp_trk");
				head[TCP_GS] = 	input.findHeading("tcp_gs");
				head[TCP_VS] = 	input.findHeading("tcp_vs");
				head[ACC_TRK] = altHeadings("acc_trk", "trk_accel", "accel_trk");
				head[ACC_GS] = 	altHeadings("acc_gs", "gs_accel", "accel_gs");
				head[ACC_VS] =	altHeadings("acc_vs", "vs_accel", "accel_vs");
				head[TRK] =	altHeadings("trk", "v_trk", "v_x","track");
				head[GS] =	altHeadings("gs", "v_gs", "v_y", "groundspeed");
				head[VS] =	altHeadings("vs", "v_vs", "v_z", "verticalspeed");
				head[SRC_LAT_SX] =	altHeadings("src_x", "src_sx", "src_lat", "src_latitude");
				head[SRC_LON_SY] =	altHeadings("src_y", "src_sy", "src_lon", "src_longitude");
				head[SRC_ALT] =		altHeadings("src_z", "src_sz", "src_alt", "src_altitude");
				head[SRC_TIME] =    altHeadings("src_clock", "src_time", "src_tm", "src_t");
				head[RADIUS] =    altHeadings("radius", "turn_radius", "R");
				head[CENTER_LAT_SX] = altHeadings("center_x", "center_lat");
				head[CENTER_LON_SY] = altHeadings("center_y", "center_lon");
				head[CENTER_ALT] =    altHeadings("center_z", "center_alt");

				head[ENDTIME] = altHeadings("endtime", "end_time", "poly_end");

				// make sure all tcp columns are defined
				if (	   head[TCP_TRK] < 0 || head[TCP_GS] < 0 || head[TCP_VS] < 0
						|| (head[ACC_TRK] < 0 && head[RADIUS] < 0)
						|| head[ACC_GS] < 0 || head[ACC_VS] < 0
						//|| head[TRK] < 0 || head[GS] < 0 || head[VS] < 0
						|| head[SRC_LAT_SX] < 0 || head[SRC_LON_SY] < 0 || head[SRC_ALT] < 0 || head[SRC_TIME] < 0) {
					if (	   head[TCP_TRK] >= 0 || head[TCP_GS] >= 0 || head[TCP_VS] >= 0
							|| head[ACC_GS] >= 0 || head[ACC_VS] >= 0
							|| head[SRC_LAT_SX] >= 0 || head[SRC_LON_SY] >= 0 || head[SRC_ALT] >= 0 || head[SRC_TIME] >= 0)
					{
						String missing = "";
						int i;
						i = TCP_TRK;
						if (head[i] < 0) missing += " "+f.Fm0(i);
						i = TCP_GS;
						if (head[i] < 0) missing += " "+f.Fm0(i);
						i = TCP_VS;
						if (head[i] < 0) missing += " "+f.Fm0(i);
						i = ACC_GS;
						if (head[i] < 0) missing += " "+f.Fm0(i);
						i = ACC_VS;
						if (head[i] < 0) missing += " "+f.Fm0(i);
						i = SRC_LAT_SX;
						if (head[i] < 0) missing += " "+f.Fm0(i);
						i = SRC_LON_SY;
						if (head[i] < 0) missing += " "+f.Fm0(i);
						i = SRC_ALT;
						if (head[i] < 0) missing += " "+f.Fm0(i);
						i = SRC_TIME;
						if (head[i] < 0) missing += " "+f.Fm0(i);
						i = RADIUS;
						if (head[i] < 0) missing += " "+f.Fm0(i);
						error.addWarning("Ignoring incorrect or incomplete TCP headers:"+missing);
					}
					tcpinfo = false;
				}

				// set accuracy parameters
				if (this.getParametersRef().contains("horizontalAccuracy")) {
					Constants.set_horizontal_accuracy(this.getParametersRef().getValue("horizontalAccuracy","m"));
				}
				if (this.getParametersRef().contains("verticalAccuracy")) {
					Constants.set_vertical_accuracy(this.getParametersRef().getValue("verticalAccuracy","m"));
				}
				if (this.getParametersRef().contains("timeAccuracy")) {
					Constants.set_time_accuracy(this.getParametersRef().getValue("timeAccuracy","s"));
				}

				if (this.getParametersRef().contains("containment")) {
					containmentList = getParametersRef().getListString("containment");
				}

				//				if (this.getParametersRef().contains("filetype")) {
				//					String sval = this.getParametersRef().getString("filetype");
				//					if (!sval.equalsIgnoreCase("plan") && !sval.equalsIgnoreCase("trajectory") &&
				//							!sval.equalsIgnoreCase("poly") && !sval.equalsIgnoreCase("plan+poly")) {
				//						error.addError("Wrong filetype: "+sval);
				//						break;
				//					}
				//				}


				if (this.getParametersRef().contains("pathMode")) {
					try {
						pathmode = PolyPath.PathMode.valueOf(this.getParametersRef().getString("pathMode").toUpperCase());
						if ((pathmode == PolyPath.PathMode.USER_VEL || pathmode == PolyPath.PathMode.USER_VEL_FINITE) && (head[TRK] < 0 || head[GS] < 0 || head[VS] < 0)) {
							error.addError("Pathmode USER_VEL does not have velocity data, reverting to pathmode MORPHING");
							pathmode = PolyPath.PathMode.MORPHING;
						}
					} catch (Exception e) {
						error.addError("Unrecognized pathmode parameter: "+this.getParametersRef().getString("pathMode")+", defaulting to MORPHING");
					}
				}

				//	              if (this.getParameters().contains("plantype")) {
				//	            	  String sval = this.getParameters().getString("plantype");
				//	                  if (sval.equalsIgnoreCase("kinematic")) {
				//	                	  kinematic = true;
				//	                  }
				//	              }

				hasRead = true;
				for (int i = 0; i <= TIME; i++) {
					if (head[i] < 0) error.addError("This appears to be an invalid poly/plan file (missing header definitions)");
				}
			}

			// determine what type of data this is...
			int linetype = UNKNOWN;
			if (input.columnHasValue(head[NAME]) && 
					input.columnHasValue(head[LAT_SX]) && 
					input.columnHasValue(head[LON_SY]) &&
					input.columnHasValue(head[SZ]) &&
					input.columnHasValue(head[TIME])) {
				linetype = PLAN;
				if (input.columnHasValue(head[SZ2])) {
					linetype = POLY;
				}
			} else {
				error.addError("Invalid data line "+input.lineNumber());
			}

			String thisName = input.getColumnString(head[NAME]);
			double myTime = getClock(input.getColumnString(head[TIME]));
			//f.pln(input.getLine());
			if ((!thisName.equals(name) )//|| lastTime > myTime) 
					&& !thisName.equals("\"")) {
				if (linetype == POLY) {
					if (containmentList.contains(thisName)) {
						containmentLine = true;
						int i = containmentNameIndex(thisName);
						if (i >= 0 && name.equals("\"")) {
							error.addWarning("Possible re-declaration of containment "+thisName);
						}
						name = thisName;
						if (i < 0) {// || lastTime > myTime) {
							containmentIndex = containment.size();
							PolyPath pp = new PolyPath(name);
							pp.setPathMode(pathmode);
							containment.add(pp);
						} else {
							containmentIndex = i;
						}
					} else {
						containmentLine = false;
						int i = pathNameIndex(thisName);
						if (i >= 0 && name.equals("\"")) {
							error.addWarning("Possible re-declaration of poly "+thisName);
						}
						name = thisName;
						if (i < 0) {// || lastTime > myTime) {
							pathIndex = paths.size();
							PolyPath pp = new PolyPath(name);
							pp.setPathMode(pathmode);
							paths.add(pp);
						} else {
							pathIndex = i;
						}
					}
				} else if (linetype == PLAN) {
					int i = planNameIndex(thisName);
					if (i >= 0 && name.equals("\"")) {
						error.addWarning("Possible re-declaration of aircraft "+thisName);
					}					
					name = thisName;
					if (i < 0) {// || lastTime > myTime) {
						planIndex = plans.size();
						Plan nplan = new Plan(name);
						if (getParametersRef().contains(name+"_note")) {
							nplan.setNote(getParametersRef().getString(name+"_note"));
						}
						plans.add(nplan);
					} else {
						planIndex = i;
					}
				}
			}

			if (pathIndex < 0 && planIndex < 0) {
				error.addError("Cannot find first entry");
				paths.clear();
				plans.clear();
				break;
			}

			if (input.hasError()) {
				error.addError(input.getMessage());
				paths.clear();
				plans.clear();
				break;
			}

			Position pos;
			if (latlon) {
				pos = Position.makeLatLonAlt(
						input.getColumn(head[LAT_SX], "deg"), "unspecified", // getColumn(_deg, head[LAT_SX]),
						input.getColumn(head[LON_SY], "deg"), "unspecified", // getColumn(_deg, head[LON_SY]),
						input.getColumn(head[SZ],      "ft"), "unspecified" // getColumn(_ft, head[SZ]),
						);
			} else {
				pos = Position.makeXYZ(
						input.getColumn(head[LAT_SX], "nmi"), "unspecified", // getColumn(_deg, head[LAT_SX]),
						input.getColumn(head[LON_SY], "nmi"), "unspecified", // getColumn(_deg, head[LON_SY]),
						input.getColumn(head[SZ],      "ft"), "unspecified"  // getColumn(_ft, head[SZ]),
						);
			}

			if (linetype == POLY) {
				double top = input.getColumn(head[SZ2], "ft");

				if (containmentLine) {
					containment.get(containmentIndex).addVertex(pos,top,myTime);

					if ((pathmode == PolyPath.PathMode.USER_VEL || pathmode == PolyPath.PathMode.USER_VEL_FINITE) && input.columnHasValue(head[TRK]) && input.columnHasValue(head[GS]) && input.columnHasValue(head[VS])) {
						Velocity vi = Velocity.makeTrkGsVs(input.getColumn(head[TRK]),input.getColumn(head[GS]),input.getColumn(head[VS]));
						containment.get(containmentIndex).setVelocity(containment.get(containmentIndex).getSegment(myTime), vi);
					}

					if (input.columnHasValue(head[ENDTIME])) {
						containment.set(containmentIndex, containment.get(containmentIndex).truncate(input.getColumn(head[ENDTIME])));
					}
					
					if (containment.get(containmentIndex).hasError()) {
						error.addError(containment.get(containmentIndex).getMessage());
						containment.clear();
						break;
					} else if (containment.get(containmentIndex).hasMessage()) {
						error.addWarning(containment.get(containmentIndex).getMessage());
					}
				} else {
					paths.get(pathIndex).addVertex(pos,top,myTime);

					if ((pathmode == PolyPath.PathMode.USER_VEL || pathmode == PolyPath.PathMode.USER_VEL_FINITE) && input.columnHasValue(head[TRK]) && input.columnHasValue(head[GS]) && input.columnHasValue(head[VS])) {
						Velocity vi = Velocity.makeTrkGsVs(input.getColumn(head[TRK]),input.getColumn(head[GS]),input.getColumn(head[VS]));
						paths.get(pathIndex).setVelocity(paths.get(pathIndex).getSegment(myTime), vi);
					}

					if (input.columnHasValue(head[ENDTIME])) {
						paths.set(pathIndex, paths.get(pathIndex).truncate(input.getColumn(head[ENDTIME])));
					}

					if (paths.get(pathIndex).hasError()) {
						error.addError(paths.get(pathIndex).getMessage());
						paths.clear();
						break;
					} else if (paths.get(pathIndex).hasMessage()) {
						error.addWarning(paths.get(pathIndex).getMessage());
					}
				}
			} else if (linetype == PLAN) {
				//				String mutString = "";
				//				if (input.columnHasValue(head[MUTABLE])) mutString = input.getColumnString(head[MUTABLE]).toLowerCase();
				String label = "";
				String info = "";
				if (input.columnHasValue(head[LABEL])) label = input.getColumnString(head[LABEL]);
				if (input.columnHasValue(head[INFO])) info = input.getColumnString(head[INFO]);
				//f.pln("label label "+label);
				NavPoint nnp = new NavPoint(pos, myTime, label);
				TcpData n = new TcpData().setInformation(info);

				if (input.columnHasValue(head[TYPE])) {
					try {
						TcpData.WayType ty = TcpData.WayType.valueOf(input.getColumnString(head[TYPE]));
						if (ty == TcpData.WayType.Virtual) {
							n = n.setVirtual();
						} else if (ty == TcpData.WayType.AltPreserve) {
							n = n.setAltPreserve();
						}
					} catch (Exception e) {
						error.addError("Unrecognized NavPoint WayType: "+input.getColumnString(head[TYPE]));
					}
				}

				if (input.columnHasValue(head[RADIUS])) {
					double rad = input.getColumn(head[RADIUS], "nmi"); 
					n = n.setRadiusSigned(rad);
				}
				Position turnCenter = Position.INVALID;
				if (input.columnHasValue(head[CENTER_LAT_SX])) {			
					if (latlon) {
						turnCenter = Position.makeLatLonAlt(
								input.getColumn(head[CENTER_LAT_SX], "deg"), "unspecified", // getColumn(_deg, head[LAT_SX]),
								input.getColumn(head[CENTER_LON_SY], "deg"), "unspecified", // getColumn(_deg, head[LON_SY]),
								input.getColumn(head[CENTER_ALT],     "ft"), "unspecified" // getColumn(_ft, head[SZ]),
								);
					} else {
						turnCenter = Position.makeXYZ(
								input.getColumn(head[CENTER_LAT_SX], "NM"), "unspecified", // getColumn(_deg, head[LAT_SX]),
								input.getColumn(head[CENTER_LON_SY], "NM"), "unspecified", // getColumn(_deg, head[LON_SY]),
								input.getColumn(head[CENTER_ALT],    "ft"), "unspecified"  // getColumn(_ft, head[SZ]),
								);
					}
					n = n.setTurnCenter(turnCenter);
					//f.pln("\n $$$$ PlanReader.loadfile: SET setTurnCenter ="+turnCenter);
				}

				if ( ! tcpinfo) {
					Pair<TcpData,String> p = n.parseMetaDataLabel(nnp,label);
					TcpData n2 = p.first;
					if (n2.isInvalid()) {
						error.addError("Plan file uses invalid metadata format");
					} else {
						n=n2;
						nnp = nnp.makeLabel(p.second);
					}
				} else { // read tcp columns
					Position srcpos = Position.INVALID;
					if (input.columnHasValue(head[SRC_LAT_SX])) {
						if (latlon) {
							srcpos = Position.makeLatLonAlt(
									input.getColumn(head[SRC_LAT_SX], "deg"), "unspecified", // getColumn(_deg, head[LAT_SX]),
									input.getColumn(head[SRC_LON_SY], "deg"), "unspecified", // getColumn(_deg, head[LON_SY]),
									input.getColumn(head[SRC_ALT],     "ft"), "unspecified" // getColumn(_ft, head[SZ]),
									);
						} else {
							srcpos = Position.makeXYZ(
									input.getColumn(head[SRC_LAT_SX], "NM"), "unspecified", // getColumn(_deg, head[LAT_SX]),
									input.getColumn(head[SRC_LON_SY], "NM"), "unspecified", // getColumn(_deg, head[LON_SY]),
									input.getColumn(head[SRC_ALT],    "ft"), "unspecified"  // getColumn(_ft, head[SZ]),
									);
						}
						if (srcpos.isInvalid()) {
							error.addWarning("Invalid source data for NavPoint "+thisName+" at "+myTime);
						}
					}
					double srcTime = input.getColumn(head[SRC_TIME], "s");
					n = n.setSource(srcpos,  srcTime);
//					NavPoint np = n.makePosition(srcpos).makeTime(srcTime);
					Velocity vel = Velocity.INVALID;
					if (input.columnHasValue(head[TRK])) {
						if (trkgsvs) {
							vel = Velocity.makeTrkGsVs(
									input.getColumn(head[TRK], "deg"), "unspecified", // getColumn(_deg, head[LAT_SX]),
									input.getColumn(head[GS],  "kts"), "unspecified", // getColumn(_deg, head[LON_SY]),
									input.getColumn(head[VS],  "fpm"), "unspecified" // getColumn(_ft, head[SZ]),
									);
						} else {
							vel = Velocity.makeVxyz(
									input.getColumn(head[TRK], "kts"), // getColumn(_deg, head[LAT_SX]),
									input.getColumn(head[GS],  "kts"), "unspecified", // getColumn(_deg, head[LON_SY]),
									input.getColumn(head[VS],  "fpm"), "unspecified"  // getColumn(_ft, head[SZ]),
									);
						}
						if (vel.isInvalid()) {
							error.addWarning("Invalid velocity data for NavPoint "+thisName+" at "+myTime);
						}
					}
					//n = n.setVelocityInit(vel);
				
					try {
						TcpData.TrkTcpType tcptrk = TcpData.TrkTcpType.valueOf(input.getColumnString(head[TCP_TRK]));
						double sRadius = n.getRadiusSigned();
						//if (input.columnHasValue(head[RADIUS])) sRadius = input.getColumn(head[RADIUS], "NM");
						if (Util.almost_equals(sRadius,0.0) && input.columnHasValue(head[ACC_TRK])) {
							double acctrk = input.getColumn(head[ACC_TRK], "deg/s");
							sRadius = vel.gs()/acctrk;
						}
						// TODO: linearIndex
						if ((tcptrk == TcpData.TrkTcpType.BOT || tcptrk == TcpData.TrkTcpType.EOTBOT) && turnCenter.isInvalid()) {					
							// This relies on the file having a TRK column !!
							//f.pln(" $$$$ PlanReader.loadfile: pos = "+pos+" sRadius = "+sRadius+" vel = "+vel);
							turnCenter = KinematicsPosition.centerFromRadius(pos, sRadius, vel.trk());
							//f.pln(" $$$$ PlanReader.loadfile: turnCenter = "+turnCenter);
						}						
						switch (tcptrk) {
						case BOT: n = n.setBOT( sRadius,turnCenter,-1); break;
						case EOT: n = n.setEOT(-1); break;
						case EOTBOT: n = n.setEOTBOT(sRadius,turnCenter,-1); break;
						default: // no change
						}
					} catch (Exception e) {
						error.addError("Unrecognized Trk_TCPType: "+input.getColumnString(head[TCP_TRK]));
					}
					try {
						TcpData.GsTcpType tcpgs = TcpData.GsTcpType.valueOf(input.getColumnString(head[TCP_GS]));
						double accgs = input.getColumn(head[ACC_GS], "m/s^2");
						// TODO: linearIndex
						switch (tcpgs) {
						case BGS: n = n.setBGS(accgs, -1); break;
						case EGS: n = n.setEGS(-1); break;
						case EGSBGS: n = n.setEGSBGS(accgs, -1); break;
						default: // no change
						}
					} catch (Exception e) {
						error.addError("Unrecognized Gs_TCPType: "+input.getColumnString(head[TCP_GS]));
					}
					try {
						TcpData.VsTcpType tcpvs = TcpData.VsTcpType.valueOf(input.getColumnString(head[TCP_VS]));
						double accvs = input.getColumn(head[ACC_VS], "m/s^2");
						// TODO: linearIndex
						switch (tcpvs) {
						case BVS: n = n.setBVS(accvs, -1); break;
						case EVS: n = n.setEVS(-1); break;
						case EVSBVS: n = n.setEVSBVS(accvs, -1); break;
						default: // no change
						}
					} catch (Exception e) {
						error.addError("Unrecognized Vs_TCPType: "+input.getColumnString(head[TCP_VS]));
					}
				}

				plans.get(planIndex).add(nnp,n);

				//if (!label.equals(PlanUtil.convertOldTCPLabel(label))) convert = true;
				if (plans.get(planIndex).hasError()) {
					error.addError(plans.get(planIndex).getMessage());
					plans.clear();
					break;
				} else if (plans.get(planIndex).hasMessage()) {
					error.addWarning(plans.get(planIndex).getMessage());
				}
			}

		} //while

//f.pln("checkpaths");
		for (int i = 0; i < paths.size(); i++) {
			PolyPath path = paths.get(i);
//			path.cleanup();
			if (!path.validate()) {
				error.addError("Path "+i+" "+path.getName()+":"+path.getMessage());
			}
		}

		for (int i = 0; i < containment.size(); i++) {
			PolyPath path = containment.get(i);
//			path.cleanup();
			if (!path.validate()) {
				error.addError("Containment area "+i+" "+path.getName()+":"+path.getMessage());
			}
		}

		//		// check if we have kinematic plans with no tcps (this is not actually a problem, but is unlikely)
		//		if (kinematic) {
		//			for (int i = 0; i < plans.size(); i++) {
		//				boolean hastcp = false;
		//				for (int j = 0; j < plans.get(i).size(); j++) {
		//					if (plans.get(i).point(j).isTCP()) {
		//						hastcp = true;
		//						break;
		//					}
		//				}
		//				if (!hastcp) {
		//					error.addWarning("Plan "+plans.get(i).getName()+" is marked as kinematic but has no TCP data");
		//				}
		//			}
		//		}

		//if (convert) {
		//	try{
		//System.err.println(">>>>>> MODIFYING <<<<<<  "+file);
		//		PrintWriter out = new PrintWriter(new BufferedWriter(new FileWriter(file, true)));
		//	    out.println("------------------------------------------------------------------------");
		//		for (int i = 0; i < plans.size(); i++) {
		//			out.println(plans.get(i).toOutput());
		//		}
		//		out.close();
		//	}catch (IOException e) {
		//		f.pln(""+e);
		//	    //exception handling left as an exercise for the reader
		//	}
		//}

		// reset accuracy parameters to their previous values
		Constants.set_horizontal_accuracy(h);
		Constants.set_vertical_accuracy(v);
		Constants.set_time_accuracy(t);
	}

	protected int planNameIndex(String name) {
		for (int i = 0; i < plans.size(); i++) {
			if (plans.get(i).getName().equals(name)) 
				return i;
		}
		return -1;
	}

	protected int altHeadings(String s1, String s2, String s3, String s4) {
		//f.pln(" $$$ altHeadings: "+s1+" "+s2+" "+s3+" "+s4);
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
		//f.pln(" $$$ altHeadings: r = "+r);
		return r;
	}

	protected int altHeadings(String s1, String s2, String s3) {
		return altHeadings(s1,s2,s3,"");
	}

	protected int altHeadings(String s1, String s2) {
		return altHeadings(s1,s2,"","");
	}


	protected double getClock(String s) {
		double tm = 0.0;
		try {
			if (clock) {
				String patternStr = "[:]";
				String [] fields2 = s.split(patternStr);
				tm = Util.parse_double(fields2[2]) + 60 * Util.parse_double(fields2[1]) + 3600 * Util.parse_double(fields2[0]);
			} else {
				tm = input.getColumn(head[TIME], "s");  //getColumn(_sec, head[TIME]);
			}
		} catch (NumberFormatException e) {
			error.addError("error parsing time at line "+input.lineNumber());
		}
		return tm;
	}

	
	protected int pathNameIndex(String name) {
		for (int i = 0; i < paths.size(); i++) {
			if (paths.get(i).getName().equals(name)) 
				return i;
		}
		return -1;
	}

	protected int containmentNameIndex(String name) {
		for (int i = 0; i < containment.size(); i++) {
			if (containment.get(i).getName().equals(name)) 
				return i;
		}
		return -1;
	}

	private int altHeadings5(String s1, String s2, String s3, String s4, String s5) {
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


	/** Return the number of paths in the file */
	public int polySize() {
		return paths.size();
	}

	/** Return the number of containment areas in the file */
	public int containmentSize() {
		return containment.size();
	}

	/** Return the number of plans in the file */
	public int planSize() {
		return plans.size();
	}


	/** Returns the i-th path in the file */
	public PolyPath getPolyPath(int i) {
		return paths.get(i);
	}

	/** Returns the i-th containment polygon in the file */
	public PolyPath getContainmentPolygon(int i) {
		return containment.get(i);
	}

	/** Returns the total number of both plans and paths. */
	public int combinedSize() {
		return plans.size() + paths.size();
	}


	/** Returns a plan or the plan corresponding to a path.  Real plans occur first in the list. 
	 * This should only be used if approximating plans by variable-sized traffic.
	 * 
	 * @param i
	 * @return
	 */
	public Plan getCombinedPlan(int i) {
		if (i < plans.size()) {
			return plans.get(i);
		} else {
			return paths.get(i - plans.size()).buildPlan().first;
		}
	}
	
	/** Return the number of plans in the file */
	public int size() {
		return plans.size();
	}

	/** Returns the i-th plan in the file */
	public Plan getPlan(int i) {
		return plans.get(i);
	}


	// ParameterReader methods


	/**
	 * Return the parameter database
	 */
	private ParameterData getParametersRef() {
		return input.getParametersRef();
	}

	public ParameterData getParameters() {
		return new ParameterData(input.getParametersRef());
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
		return input.toString();
	}


}
