/* 
 * PlanReader
 *
 * Note: if the accuracy parameters (horizontalAccuracy, verticalAccuracy, timeAccuracy) 
 * are changed in a file, ONLY THOSE will be interpreted here (and values will be re-set
 * afterwards). This is necessary to correctly read in a Plan.  They may need to be 
 * explicitly set later to do any manipulations of the Plans.
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
import gov.nasa.larcfm.Util.NavPoint;
import gov.nasa.larcfm.Util.ParameterData;
import gov.nasa.larcfm.Util.ParameterProvider;
import gov.nasa.larcfm.Util.Plan;
import gov.nasa.larcfm.Util.Position;
import gov.nasa.larcfm.Util.Util;
import gov.nasa.larcfm.Util.Velocity;
import gov.nasa.larcfm.Util.f;
import gov.nasa.larcfm.Util.NavPoint.Gs_TCPType;
import gov.nasa.larcfm.Util.NavPoint.Trk_TCPType;
import gov.nasa.larcfm.Util.NavPoint.Vs_TCPType;
import gov.nasa.larcfm.Util.NavPoint.WayType;

import java.io.*;
import java.util.ArrayList;

/**
 * This reads in and stores a set of plans (and parameters) from a text file.
 * Plan files consist of comma or space-separated values, with one point per line.
 * Required columns include either:<br>
 * Aircraft name, x-position [nmi], y-position [nmi], z-position [ft], time [s]<br>
 * or<br>
 * Aircraft name, latitude [deg], longitude [deg], altitude [ft], time [s]<p>
 *
 * Optional columns include point mutability and point label, both with [unspecified] units.  Even if these columns are defined, 
 * these two values are optional for any given line.<p>
 *
 * It is necessary to include a header line that defines the column ordering.  The column definitions are not case sensitive.
 * There is also an optional header line, immediately following the column definition, that defines the unit type for each
 * column (the defaults are listed above).<p>
 *
 * If points are consecutive for the same aircraft, subsequent name fields may be replaced with a double quotation mark (").
 * Assume the aircraft name is case sensitive, so US54A != Us54a != us54a.<p>
 *
 * Any empty line or any line starting with a hash sign (#) is ignored.<p>
 *
 * Files may also include parameter definitions prior to other data.  Parameter definitions are of the form <key> = <value>,
 * one per line, where <key> is a case-insensitive alphanumeric word and <value> is either a numeral or string.  The <value> 
 * may include a unit, such as "dist = 50 [m]".  Note that parameters require a space on both sides of the equals sign.<p>
 *
 * Parameters can be interpreted as double values, strings, or Boolean values, and the user is required to know which parameter is
 * interpreted as which type.<p>
 * 
 * If the optional parameter "filetype" is specified, its value must be "plan" or "trajectory" (no quotes) for this reader to accept the 
 * file without error.<p>
 * 
 * Plan "note" information is to be stored with a parameter named ID_note, where ID is the name of the plan in question.
 * 
 */
public class PlanReader implements ParameterProvider, /*ParameterReader,*/ ErrorReporter {
	protected ErrorLog error;
	protected SeparatedInput input;
	protected ArrayList<Plan> plans = new ArrayList<Plan>();
	protected boolean clock;
	protected int[] head = new int[NavPoint.TCP_OUTPUT_COLUMNS+1]; // array size of number of elements (vector in C++)
	// we store the heading indices in the following order:
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
	protected final int RADIUS = 20; 

	public PlanReader() {
		error = new ErrorLog("PlanReader()");
		//states = new ArrayList<AircraftState>(0);
		input = new SeparatedInput();
		input.setCaseSensitive(false);            // headers & parameters are lower case
		//fname = "";
	}

	/** Create a new PlanReader reading the specified file. */
	public void open(String filename) {
		error = new ErrorLog("PlanReader("+filename+")");
		if (filename == null || filename.equals("")) {
			error.addError("Empty file name");			
			return;
		}
		FileReader fr;
		try {
			fr = new FileReader(filename);
		} catch (FileNotFoundException e) {
			input = new SeparatedInput();
			error.addError("File "+filename+" read protected or not found");
			if (plans != null) plans.clear();
			return;
		}
		open(fr);
	}

	public void open(Reader r) {
		if (r == null) {
			error.addError("null given for open(Reader)");
			return;
		}
		input = new SeparatedInput(r);
		input.setCaseSensitive(false);            // headers & parameters are lower case
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
		String name = ""; // the current aircraft name
		//		double lastTime = -1000000; // time must be increasing
		int planIndex = -1;

		input.setCaseSensitive(false);            // headers & parameters are lower case

		// save accuracy info in temp vars
		double h = Constants.get_horizontal_accuracy();
		double v = Constants.get_vertical_accuracy();
		double t = Constants.get_time_accuracy();

		//        boolean kinematic = false;

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
				head[SZ] =      altHeadings("sz", "alt", "altitude");
				head[TIME] =    altHeadings("clock", "time", "tm", "st");
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
				head[SRC_TIME] =    altHeadings("src_time", "src_tm", "src_t");
				head[RADIUS] =    altHeadings("radius", "turn_radius", "R");


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
					if (!sval.equalsIgnoreCase("plan") && !sval.equalsIgnoreCase("trajectory")) {
						error.addError("Wrong filetype: "+sval);
						break;
					}
				}

				//              if (getParameters().contains("plantype")) {
				//            	  String sval = getParameters().getString("plantype");
				//                  if (sval.equalsIgnoreCase("kinematic")) {
				//                	  kinematic = true;
				//                  }
				//              }

				hasRead = true;
				for (int i = 0; i <= TIME; i++) {
					if (head[i] < 0) error.addError("This appears to be an invalid plan file (missing header definitions)");
				}
			}

			String thisName = input.getColumnString(head[NAME]);
			double myTime = getClock(input.getColumnString(head[TIME]));

			if ((!thisName.equals(name) )//|| lastTime > myTime)
					&& !thisName.equals("\"")) {
				int i = planNameIndex(thisName);
				if (i >= 0 && name.equals("\"")) {
					error.addWarning("Possible re-declaration of aircraft "+thisName);
				}
				name = thisName;
				if (i < 0 ){//|| lastTime > myTime) {
					planIndex = plans.size();
					Plan nplan = new Plan(name);
					// add note, if present
					if (getParametersRef().contains(name+"_note")) {
						nplan.setNote(getParametersRef().getString(name+"_note"));
					}
					
					//					if (kinematic) {
					//						nplan.setPlanType(PlanType.KINEMATIC);
					//					}
					plans.add(nplan);
				} else {
					planIndex = i;
				}
			}

			if (planIndex < 0) {
				error.addError("Cannot find first aircraft");
				plans.clear();
				break;
			}

			if (input.hasError()) {
				error.addError(input.getMessage());
				plans.clear();
				break;
			}

			//			String mutString = "";
			//			if (input.columnHasValue(head[MUTABLE])) mutString = input.getColumnString(head[MUTABLE]).toLowerCase();
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
			String label = "";
			if (input.columnHasValue(head[LABEL])) label = input.getColumnString(head[LABEL]);

			NavPoint n = new NavPoint(
					pos, 
					myTime,
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
								input.getColumn(head[SRC_ALT],     "ft"), "unspecified" // getColumn(_ft, head[SZ]),
								);
					} else {
						srcpos = Position.makeXYZ(
								input.getColumn(head[SRC_LAT_SX], "NM"), "unspecified", // getColumn(_deg, head[LAT_SX]),
								input.getColumn(head[SRC_LON_SY], "NM"), "unspecified", // getColumn(_deg, head[LON_SY]),
								input.getColumn(head[SRC_ALT],    "ft"), "unspecified"  // getColumn(_ft, head[SZ]),
								);
					}
				}
				double srcTime = input.getColumn(head[SRC_TIME], "s");
				n = n.makeSource(srcpos,  srcTime);
				//NavPoint np = n.makePosition(srcpos).makeTime(srcTime);
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

			//			lastTime = myTime;
			if (plans.get(planIndex).hasError()) {
				error.addError(plans.get(planIndex).getMessage());
				plans.clear();
				break;
			} else if (plans.get(planIndex).hasMessage()) {
				error.addWarning(plans.get(planIndex).getMessage());
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


	//	// i is 0, 1, 2
	//	public static NavPoint.Mutability getMutability(String s, int i) {
	//		if (s.length() == 0) return NavPoint.Mutability.Mutable;
	//		if (i >= s.length()) return getMutability(s,0);
	//		switch (s.charAt(i)) {
	//		case '0':
	//		case 'f': return NavPoint.Mutability.Fixed;
	//		case '1':
	//		case 'm': return NavPoint.Mutability.Mutable;
	//		default: return NavPoint.Mutability.Mutable;
	//		}
	//	}

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

	//    /** Convert a NavPoint to a TCP based on a string qualifier */
	//	protected NavPoint setTcpType(NavPoint n, String s) {
	//		try {
	//		  switch(NavPoint.Trk_TCPType.valueOf(s.toUpperCase())) {
	//		  case BOT: return n.makeTrkTCP(Trk_TCPType.BOT); // n.makeTurnBegin();
	////		  case EOT: return n.makeTCPTurnEnd();
	////		  case BGSC: return n.makeTCPGSCBegin();
	////		  case EGSC: return n.makeTCPGSCEnd();
	////		  case BVSC: return n.makeTCPVSCBegin();
	////		  case EVSC: return n.makeTCPVSCEnd();
	////		  case TMID: return n.makeTCPTurnMid();
	//		  default: return n;
	//		  
	//		  }
	//		} catch (Exception e) {
	//			return n;
	//		}
	//	}

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
