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
 * Copyright (c) 2011-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 *  */

package gov.nasa.larcfm.IO;


import gov.nasa.larcfm.Util.Constants;
import gov.nasa.larcfm.Util.ErrorLog;
import gov.nasa.larcfm.Util.NavPoint;
import gov.nasa.larcfm.Util.ParameterData;
import gov.nasa.larcfm.Util.Plan;
import gov.nasa.larcfm.Util.PolyPath;
import gov.nasa.larcfm.Util.Position;
import gov.nasa.larcfm.Util.Util;
import gov.nasa.larcfm.Util.Velocity;
import gov.nasa.larcfm.Util.f;
import gov.nasa.larcfm.Util.NavPoint.Gs_TCPType;
import gov.nasa.larcfm.Util.NavPoint.Trk_TCPType;
import gov.nasa.larcfm.Util.NavPoint.Vs_TCPType;
import gov.nasa.larcfm.Util.NavPoint.WayType;
import gov.nasa.larcfm.Util.PolyPath.PathMode;

import java.io.*;
import java.util.ArrayList;
import java.util.List;

//import java.util.Arrays;
//import java.util.Collection;
//import gov.nasa.larcfm.Util.*;
//import static gov.nasa.larcfm.Util.UnitSymbols.*;

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
 * (time of entry n) < (time of entry n+1).  The type of the entry is defined by the presence or lack of a second 
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
 * Files may also include parameter definitions prior to other data.  Parameter definitions are of the form <key> = <value>,
 * one per line, where <key> is a case-insensitive alphanumeric word and <value> is either a numeral or string.  The <value> 
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
public class PolyReader extends PlanReader { //implements ParameterReader, ErrorReporter {
	protected ArrayList<PolyPath> paths = new ArrayList<PolyPath>();
	private ArrayList<PolyPath> containment = new ArrayList<PolyPath>();
	// we store the heading indices in the following order:
	private final int SZ2 = 21;
	private final int ENDTIME = 22;

	// data line types
	private final int POLY = -1;
	private final int UNKNOWN = 0;
	private final int PLAN = 1;


	//private boolean interpretUnits;
	//String file;
	/** Create a new PolyReader reading the specified file. */
	public PolyReader(String filename) {
		//file = filename;		
		head = new int[NavPoint.TCP_OUTPUT_COLUMNS+3]; // +name +polytop +endtime
		open(filename);
	}
	
	public PolyReader() {
		head = new int[NavPoint.TCP_OUTPUT_COLUMNS+3]; // +name +polytop + endtime
	}
	
	public void open(String filename) {
		error = new ErrorLog("PolyReader("+filename+")");		
		if (filename == null || filename.equals("")) {
			input = new SeparatedInput();
			error.addError("Empty file name.");
			return;
		}
		SeparatedInput si;
		try {
			FileReader fr = new FileReader(filename);
			si = new SeparatedInput(fr);
		} catch (FileNotFoundException e) {
			input = new SeparatedInput();
			error.addError("File "+filename+" read protected or not found");
			if (plans != null) plans.clear();
			if (paths != null) paths.clear();
			if (containment != null) containment.clear();
			return;
		}
		input = si;
		loadfile();
	}

	public PolyReader(SeparatedInput si) {
		head = new int[10];
		error = new ErrorLog("PolyReader(SeparatedInput)");
		if (si == null) {
			return;
		}
		input = si;
		loadfile();
	}

	// open(Reader r) is inherited from PlanReader
	
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
				head[LABEL] =   input.findHeading("label");

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
				
				head[ENDTIME] = altHeadings("endtime", "end_time", "poly_end");

				// make sure all tcp columns are defined
				if (	   head[TCP_TRK] < 0 || head[TCP_GS] < 0 || head[TCP_VS] < 0
						|| head[ACC_TRK] < 0 || head[ACC_GS] < 0 || head[ACC_VS] < 0
						|| head[TRK] < 0 || head[GS] < 0 || head[VS] < 0
						|| head[SRC_LAT_SX] < 0 || head[SRC_LON_SY] < 0 || head[SRC_ALT] < 0 || head[SRC_TIME] < 0) {
					if (	   head[TCP_TRK] >= 0 || head[TCP_GS] >= 0 || head[TCP_VS] >= 0
							|| head[ACC_TRK] >= 0 || head[ACC_GS] >= 0 || head[ACC_VS] >= 0
							|| head[SRC_LAT_SX] >= 0 || head[SRC_LON_SY] >= 0 || head[SRC_ALT] >= 0 || head[SRC_TIME] >= 0)
					{
						String missing = "";
						for (int i = TCP_TRK; i <= SRC_TIME; i++) {
							if (head[i] < 0) missing += " "+f.Fm0(i);
						}
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
				if (input.columnHasValue(head[LABEL])) label = input.getColumnString(head[LABEL]);
				NavPoint n = new NavPoint(
						pos, 
						myTime,
						//NavPoint.WayType.Orig, 
						label
						);

				if (input.columnHasValue(head[TYPE])) {
					try {
						NavPoint.WayType ty = NavPoint.WayType.valueOf(input.getColumnString(head[TYPE]));
						if (ty == NavPoint.WayType.Virtual) {
							n = n.makeVirtual();
						} else if (ty == NavPoint.WayType.AltPreserve) {
							n = n.makeAltPreserve();
						}
					} catch (Exception e) {
						error.addError("Unrecognized NavPoint WayType: "+input.getColumnString(head[TYPE]));
					}
				}

				if (input.columnHasValue(head[RADIUS])) {
					double rad = input.getColumn(head[RADIUS], "nmi"); 
					n = n.makeRadius(rad);
				}
			
				if (!tcpinfo) {
					NavPoint n2 = n.parseMetaDataLabel(label);
					if (n2.isInvalid()) {
						error.addError("Plan file uses invalid metadata format");
					} else {
						n=n2;
					}
				} else { // read tcp columns
					Position srcpos = Position.INVALID;
					if (input.columnHasValue(head[SRC_LAT_SX])) {
						if (latlon) {
							srcpos = Position.makeLatLonAlt(
									input.getColumn(head[SRC_LAT_SX], "deg"), "unspecified", // getColumn(_deg, head[LAT_SX]),
									input.getColumn(head[SRC_LON_SY], "deg"), "unspecified", // getColumn(_deg, head[LON_SY]),
									input.getColumn(head[SRC_ALT],      "ft"), "unspecified" // getColumn(_ft, head[SZ]),
									);
						} else {
							srcpos = Position.makeXYZ(
									input.getColumn(head[SRC_LAT_SX], "nmi"), "unspecified", // getColumn(_deg, head[LAT_SX]),
									input.getColumn(head[SRC_LON_SY], "nmi"), "unspecified", // getColumn(_deg, head[LON_SY]),
									input.getColumn(head[SRC_ALT],      "ft"), "unspecified"  // getColumn(_ft, head[SZ]),
									);
						}
						if (srcpos.isInvalid()) {
							error.addWarning("Invalid source data for NavPoint "+thisName+" at "+myTime);
						}
					}
					double srcTime = input.getColumn(head[SRC_TIME], "s");
					n = n.makeSource(srcpos,  srcTime);
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
					n = n.makeVelocityIn(vel);
				
					try {
						NavPoint.Trk_TCPType tcptrk = NavPoint.Trk_TCPType.valueOf(input.getColumnString(head[TCP_TRK]));
						double acctrk = input.getColumn(head[ACC_TRK], "deg/s");
						double sRadius = 0.0;
						if (input.columnHasValue(head[RADIUS])) sRadius = input.getColumn(head[RADIUS], "NM");
						if (Util.almost_equals(sRadius,0.0)) sRadius = vel.gs()/acctrk;
						switch (tcptrk) {
						case BOT: n = n.makeBOT(n.position(), n.time(), vel, sRadius); break;
						case EOT: n = n.makeEOT(n.position(), n.time(), vel); break;
						case EOTBOT: n = n.makeEOTBOT(n.position(), n.time(), vel, sRadius); break;
						default: // no change
						}
					} catch (Exception e) {
						error.addError("Unrecognized Trk_TCPType: "+input.getColumnString(head[TCP_TRK]));
					}
					try {
						NavPoint.Gs_TCPType tcpgs = NavPoint.Gs_TCPType.valueOf(input.getColumnString(head[TCP_GS]));
						double accgs = input.getColumn(head[ACC_GS], "m/s^2");
						switch (tcpgs) {
						case BGS: n = n.makeBGS(n.position(), n.time(), accgs, vel); break;
						case EGS: n = n.makeEGS(n.position(), n.time(), vel); break;
						case EGSBGS: n = n.makeEGSBGS(n.position(), n.time(), accgs, vel); break;
						default: // no change
						}
					} catch (Exception e) {
						error.addError("Unrecognized Gs_TCPType: "+input.getColumnString(head[TCP_GS]));
					}
					try {
						NavPoint.Vs_TCPType tcpvs = NavPoint.Vs_TCPType.valueOf(input.getColumnString(head[TCP_VS]));
						double accvs = input.getColumn(head[ACC_VS], "m/s^2");
						switch (tcpvs) {
						case BVS: n = n.makeBVS(n.position(), n.time(), accvs, vel); break;
						case EVS: n = n.makeEVS(n.position(), n.time(), vel); break;
						case EVSBVS: n = n.makeEVSBVS(n.position(), n.time(), accvs, vel); break;
						default: // no change
						}
					} catch (Exception e) {
						error.addError("Unrecognized Vs_TCPType: "+input.getColumnString(head[TCP_VS]));
					}

				}

				plans.get(planIndex).add(n);

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

	private ParameterData getParametersRef() {
		return input.getParametersRef();
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

}
