/*
 * Copyright (c) 2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
package gov.nasa.larcfm.IO;

import gov.nasa.larcfm.Util.Constants;
import gov.nasa.larcfm.Util.ErrorLog;
import gov.nasa.larcfm.Util.ErrorReporter;
import gov.nasa.larcfm.Util.GeneralPlan;
import gov.nasa.larcfm.Util.NavPoint;
import gov.nasa.larcfm.Util.Pair;
import gov.nasa.larcfm.Util.ParameterData;
import gov.nasa.larcfm.Util.ParameterProvider;
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

import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.Reader;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashSet;
import java.util.HashMap;
import java.util.Hashtable;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.Set;

/**
 * Similar to the SequenceReader, this populates a list of GeneralPlans (that can be either a Plan or a PolyPath).
 * Each GeneralPlan may include a "start_time" or "activation_time" field, orthogonal to the the normal "time" field, 
 * that indicates when the plan is designated to be transmitted or go active.
 * A given GeneralPlan id may appear with multiple activation times.
 * 
 * This allows, for example, a single file to include multiple plan revisions for aircraft that would be implemented 
 * at different times.
 * 
 * The user can only access a subset of all GeneralPlans in a file, depending on the activation time specified.
 * The method sequenceKeys() lists all activation times in the file.
 * The method setActive() will only access the set of those entries at the specific activation time (possibly empty).
 * The other setActive...() methods makes a set of the most recent entry for each id up to the given activation time,
 * so if x had entries at 100 and 200 and y had entries at 150 and 250, setActiveUpTo(200) would return the entries
 * for x at 200 and y at 150.
 *
 * By default the last activation_time plans are presented.
 * 
 * Default plan "note" information is to be stored with a parameter named ID_note, where ID is the name of the plan in question.
 * The "note" parameter for a plan at a specific activation time should follow the pattern ID_TIME_note, where TIME is 
 * the activation time truncated to the second.
 * Using the above example, x_note applies the note to both x plans, while y_250_note would only apply the note to the plan y that
 * activated at time 250.  
 *
 */
public class GeneralPlanReader implements ParameterProvider, ErrorReporter {
	protected ErrorLog error;
	protected SeparatedInput input;
	
	private Map< Double, Map<String, GeneralPlan > > sequenceTable = new HashMap< Double, Map<String, GeneralPlan > >();
	private ArrayList<String> nameIndex = new ArrayList<String>();
	private Set<String> names = new HashSet<String>();

	protected ArrayList<GeneralPlan> g2plans = new ArrayList<GeneralPlan>();
	
	protected String defaultOwnship = null;
	
	protected boolean clock;
	protected int[] head = new int[NavPoint.TCP_OUTPUT_COLUMNS+4]; // array size of number of elements (vector in C++)
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
	protected final int SZ2 = 21;
	protected final int TIME2 = 22; // this is an "activation time" after which the plan goes live

	protected final int ENDTIME = 22; // polygon truncation time
	
	protected final int POLY = -1;
	protected final int UNKNOWN = 0;
	protected final int PLAN = 1;

	public GeneralPlanReader() {
		error = new ErrorLog("GeneralPlanReader()");
		//states = new ArrayList<AircraftState>(0);
		input = new SeparatedInput();
		input.setCaseSensitive(false);            // headers & parameters are lower case
		//fname = "";
	}

	/** Create a new GeneralPlanReader reading the specified file. */
	public void open(String filename) {
		error = new ErrorLog("GeneralPlanReader("+filename+")");
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
			if (sequenceTable != null) sequenceTable.clear();
			return;
		}
		open(fr);
	}

	public void open(Reader r) {
		if (r == null) {
			return;
		}
		input = new SeparatedInput(r);
		input.setCaseSensitive(false);            // headers & parameters are lower case
		loadfile();
	}

	private void loadfile() {
		boolean hasRead = false;
		defaultOwnship = null;
		clock = true;
		boolean latlon = true;
		boolean tcpinfo = true;
		boolean trkgsvs = true;
		//interpretUnits = false;
		sequenceTable.clear();
//		gplans = new ArrayList<GeneralPlan>(10);
		String name = ""; // the current aircraft name
		//		double lastTime = -1000000; // time must be increasing
//		int planIndex = -1;

		input.setCaseSensitive(false);            // headers & parameters are lower case

		// save accuracy info in temp vars
		double h = Constants.get_horizontal_accuracy();
		double v = Constants.get_vertical_accuracy();
		double t = Constants.get_time_accuracy();

		PolyPath.PathMode pathmode = PolyPath.PathMode.MORPHING; 
		List<String> containmentList = new ArrayList<String>();
		String lastName = ""; // the current aircraft name

		//        boolean kinematic = false;

		while ( ! input.readLine()) {
//f.pln("GeneralPlanReaer line="+input.getLine());			
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
				head[RADIUS] = altHeadings("radius", "turn_radius", "R");
				head[TIME2] = altHeadings("start_time", "activation_time", "time2", "active_time");
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
				if (getParametersRef().contains("horizontalAccuracy")) {
					Constants.set_horizontal_accuracy(getParametersRef().getValue("horizontalAccuracy","m"));
				}
				if (getParametersRef().contains("verticalAccuracy")) {
					Constants.set_vertical_accuracy(getParametersRef().getValue("verticalAccuracy","m"));
				}
				if (getParametersRef().contains("timeAccuracy")) {
					Constants.set_time_accuracy(getParametersRef().getValue("timeAccuracy","s"));
				}

				if (this.getParametersRef().contains("containment")) {
					containmentList = getParametersRef().getListString("containment");
				}

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

//f.pln("time="+input.getColumnString(head[TIME])+"    time2="+input.getColumnString(head[TIME2]));			
			
			String thisName = input.getColumnString(head[NAME]);
			double myTime = getClock(TIME);
			double startTime = 0.0;
			if (head[TIME2] >= 0 && input.columnHasValue(head[TIME2])) {
				startTime = getClock(TIME2);
			}
						
			if (thisName.equals("\"") && !lastName.equals("")) {
				thisName = lastName;
				names.add(thisName);
			} else if ((thisName.equals("\"") && lastName.equals("")) || thisName.equals("")) {
				error.addError("Cannot find first aircraft");
				sequenceTable.clear();
				break;				
			} else if (!names.contains(thisName)) {
				lastName = thisName;
				nameIndex.add(thisName);
				names.add(thisName);
				//stateIndex++;
			}

//f.pln("GeneralPlanReader entry startTime="+startTime+" "+thisName+" myTime="+myTime);			
			Map<String, GeneralPlan > entry = sequenceTable.get(startTime);
			if (entry == null) {
				entry = new HashMap<String, GeneralPlan>();
				sequenceTable.put(startTime, entry);
			}
//f.pln("entry="+f.Fobj(entry));
//f.pln(input.getLine());

			if (!entry.containsKey(thisName)) {
				if (linetype == POLY) {
					PolyPath pp = new PolyPath(thisName);
					pp.setPathMode(pathmode);
					entry.put(thisName, new GeneralPlan(pp, containmentList.contains(thisName)));
				} else {
					Plan nplan = new Plan(thisName);
					if (defaultOwnship == null) defaultOwnship = thisName;
					String ntime = "_"+((int)startTime);
					if (getParametersRef().contains(thisName+ntime+"_note")) {
						nplan.setNote(getParametersRef().getString(thisName+ntime+"_note"));
					} else if (getParametersRef().contains(thisName+"_note")) {
						nplan.setNote(getParametersRef().getString(thisName+"_note"));
					}
					entry.put(thisName, new GeneralPlan(nplan));					
				}
				names.add(thisName);
			}

			if (!entry.containsKey(thisName)) {
				error.addError("Cannot find first entry");
				sequenceTable.clear();
				break;
			}

			if (input.hasError()) {
				error.addError(input.getMessage());
				sequenceTable.clear();
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
				entry.get(thisName).getPolyPath().addVertex(pos,top,myTime);

				if ((pathmode == PolyPath.PathMode.USER_VEL || pathmode == PolyPath.PathMode.USER_VEL_FINITE) && input.columnHasValue(head[TRK]) && input.columnHasValue(head[GS]) && input.columnHasValue(head[VS])) {
					Velocity vi = Velocity.makeTrkGsVs(input.getColumn(head[TRK]),input.getColumn(head[GS]),input.getColumn(head[VS]));
					entry.get(thisName).getPolyPath().setVelocity(entry.get(thisName).getSegment(myTime), vi);
				}

				if (input.columnHasValue(head[ENDTIME])) {
					GeneralPlan gp = entry.get(thisName);
					entry.put(thisName, new GeneralPlan(gp.getPolyPath().truncate(input.getColumn(head[ENDTIME])), gp.isContainment()));
				}

				if (entry.get(thisName).hasError()) {
					error.addError(entry.get(thisName).getMessage());
					sequenceTable.clear();
					break;
				} else if (entry.get(thisName).hasMessage()) {
					error.addWarning(entry.get(thisName).getMessage());
				}
			} else if (linetype == PLAN) {
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
					n = n.makeVelocityInit(vel);

					try {
						NavPoint.Trk_TCPType tcptrk = NavPoint.Trk_TCPType.valueOf(input.getColumnString(head[TCP_TRK]));
						double acctrk = input.getColumn(head[ACC_TRK], "deg/s");
						double radius = 0.0;
						if (input.columnHasValue(head[RADIUS])) radius = input.getColumn(head[RADIUS], "NM");
						if (Util.almost_equals(radius,0.0)) radius = vel.gs()/acctrk;
						// TODO: linearIndex
						switch (tcptrk) {
						case BOT: n = n.makeBOT(n.position(), n.time(), vel, radius,-1); break;
						case EOT: n = n.makeEOT(n.position(), n.time(), vel,-1); break;
						case EOTBOT: n = n.makeEOTBOT(n.position(), n.time(), vel, radius,-1); break;
						default: // no change
						}
					} catch (Exception e) {
						error.addError("Unrecognized Trk_TCPType: "+input.getColumnString(head[TCP_TRK]));
					}
					try {
						NavPoint.Gs_TCPType tcpgs = NavPoint.Gs_TCPType.valueOf(input.getColumnString(head[TCP_GS]));
						double accgs = input.getColumn(head[ACC_GS], "m/s^2");
						// TODO: linearIndex
						switch (tcpgs) {
						case BGS: n = n.makeBGS(n.position(), n.time(), accgs, vel,-1); break;
						case EGS: n = n.makeEGS(n.position(), n.time(), vel,-1); break;
						case EGSBGS: n = n.makeEGSBGS(n.position(), n.time(), accgs, vel,-1); break;
						default: // no change
						}
					} catch (Exception e) {
						error.addError("Unrecognized Gs_TCPType: "+input.getColumnString(head[TCP_GS]));
					}
					try {
						NavPoint.Vs_TCPType tcpvs = NavPoint.Vs_TCPType.valueOf(input.getColumnString(head[TCP_VS]));
						double accvs = input.getColumn(head[ACC_VS], "m/s^2");
						// TODO: linearIndex
						switch (tcpvs) {
						case BVS: n = n.makeBVS(n.position(), n.time(), accvs, vel,-1); break;
						case EVS: n = n.makeEVS(n.position(), n.time(), vel,-1); break;
						case EVSBVS: n = n.makeEVSBVS(n.position(), n.time(), accvs, vel,-1); break;
						default: // no change
						}
					} catch (Exception e) {
						error.addError("Unrecognized Vs_TCPType: "+input.getColumnString(head[TCP_VS]));
					}

				}

				entry.get(thisName).getPlan().add(n);

				//if (!label.equals(PlanUtil.convertOldTCPLabel(label))) convert = true;
				if (entry.get(thisName).hasError()) {
					error.addError(entry.get(thisName).getMessage());
					sequenceTable.clear();
					break;
				} else if (entry.get(thisName).hasMessage()) {
					error.addWarning(entry.get(thisName).getMessage());
				}
			}
		} //while

		for (Double key1 : sequenceTable.keySet()) {
			Map<String, GeneralPlan > entry = sequenceTable.get(key1);
			for (String key2 : entry.keySet()) {
				GeneralPlan p = entry.get(key2);
				if (!p.validate()) {
					error.addError("Plan "+key2+" "+p.getName()+" is not validating"+p.getMessage());
				}				
			}
		}

		// reset accuracy parameters to their previous values
		Constants.set_horizontal_accuracy(h);
		Constants.set_vertical_accuracy(v);
		Constants.set_time_accuracy(t);

		setLastActive();
	}

	protected int planNameIndex(String name) {
		for (int i = 0; i < g2plans.size(); i++) {
			if (g2plans.get(i).getName().equals(name)) 
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

	protected double getClock(int TIMEFIELD) {
		double tm = 0.0;
		String s = input.getColumnString(head[TIMEFIELD]);
		try {
			if (clock) {
				String patternStr = "[:]";
				String [] fields2 = s.split(patternStr);
				tm = Util.parse_double(fields2[2]) + 60 * Util.parse_double(fields2[1]) + 3600 * Util.parse_double(fields2[0]);
			} else {
				tm = input.getColumn(head[TIMEFIELD], "s");  //getColumn(_sec, head[TIME]);
			}
		} catch (NumberFormatException e) {
			error.addError("error parsing time at line "+input.lineNumber());
		}
		return tm;
	}

	/** Return the number of plans in the file */
	public int size() {
		return g2plans.size();
	}

	/** Returns the i-th plan in the file */
	public GeneralPlan getGeneralPlan(int i) {
		return g2plans.get(i);
	}

	/**
	 * Return a list of current active GeneralPlans
	 * @return
	 */
	public List<GeneralPlan> getGeneralPlanList() {
		ArrayList<GeneralPlan> plans = new ArrayList<GeneralPlan>(g2plans);
		return plans;
	}

	/**
	 * Return a list of all plans up to the indicated time.  If a plan has more than one activation time, only the one closest to the given time will be in the list.
	 * @return
	 */
	public List<GeneralPlan> getGeneralPlanListUpTo(double time) {
		ArrayList<GeneralPlan> plans = new ArrayList<GeneralPlan>();
		
		return plans;
	}

//	/** remove any time-point entries for which there is only one aircraft (and so no chance of conflict) */
//	public void clearSingletons() {
//		ArrayList<Double> keys = sequenceKeys();		
//		for (int i = 0; i < keys.size(); i++) {
//			if (sequenceTable.get(keys.get(i)).size() < 2) {
////f.pln("removing singleton (only one aircraft present) at time "+keys.get(i));				
//				sequenceTable.remove(keys.get(i));
//			}
//		}
//	}

	// we need to preserve the order of the aircraft as in the input file (because the first might be the only way we know which is the ownship)
	// so we build an arraylist states to us as the subset of all possible inputs
	private void buildActive(double tm, boolean single) {
		ArrayList<Double> times;
		if (single) {
			times = new ArrayList<Double>();
			times.add(tm);
		} else {
			 times = sequenceKeysUpTo(tm); // Note: this includes the last entry
		}
 		Map<String, Boolean> included = new HashMap<String,Boolean>(10); // use to make sure there are no duplicates
		g2plans.clear();
		// build all AircraftStates that exist in these times
		for (int i = 0; i < nameIndex.size(); i++) { // work through the names in order
			String name = nameIndex.get(i);
			for (int j = 0; j < times.size(); j++) { // for each name, work through the times in the window
				double time = times.get(j);
				Map<String, GeneralPlan> sequenceEntry = sequenceTable.get(time);
				if (sequenceEntry != null && sequenceEntry.containsKey(name)) {	// name has an entry at this time
					GeneralPlan p = sequenceEntry.get(name);	// get entry info
					if (included.containsKey(name)) {  // name has not been added to the states list yet
						int k = planNameIndex(name);
						g2plans.set(k, p);				// replace existing entry with newer entry
					} else {
						// build a new AircraftState
						included.put(name, true);				// note name has been used
						g2plans.add(p);
					} 
				}
			}
		}
	}
	
	/**
	 * Given a sequence key, set the active set of states.  If no such key exists, the active set is left empty.
	 * This only included plans that activate at the indicated time, not earlier ones.
	 * @param tm Sequence key (time)
	 */
	public void setActive(double tm) {
		g2plans.clear();
		if (sequenceTable.containsKey(tm)) {
			buildActive(tm, true);
		}
	}

	/**
	 * Given a sequence key, set the active set of states.  Includes all plans that activate at or before the indicated time.
	 * @param tm Sequence key (time)
	 */
	public void setActiveUpTo(double tm) {
		g2plans.clear();
		buildActive(tm, false);
	}

	/**
	 * Set only the first entry to be the active one.
	 */
	public void setFirstActive() {
		ArrayList<Double> keys = sequenceKeys();
		if (keys.size() > 0)
			buildActive(keys.get(0), false);
		else
			g2plans = new ArrayList<GeneralPlan>();
	}

	/**
	 * Set all entries up to the last entry to be the active one.
	 */
	public void setLastActive() {
		ArrayList<Double> keys = sequenceKeys();
		if (keys.size() > 0)
			buildActive(keys.get(keys.size()-1), false);
		else
			g2plans = new ArrayList<GeneralPlan>();
	}

	
	/**
	 *  Returns a sorted list of all sequence keys (times)
	 */
	public ArrayList<Double> sequenceKeys() {
		ArrayList<Double> arl = new ArrayList<Double>();
		for (Iterator<Double> e = sequenceTable.keySet().iterator(); e.hasNext();) {
			arl.add(e.next());
		}
		Double[] ar = arl.toArray(new Double[0]);
		Arrays.sort(ar);
		arl = new ArrayList<Double>(Arrays.asList(ar));
		return arl;
	}

	/** a list of n > 0 sequence keys, stopping at the given time (inclusive) */ 
	public ArrayList<Double> sequenceKeysUpTo(double tm) {
		ArrayList<Double> arl = new ArrayList<Double>();
		for (Iterator<Double> e = sequenceTable.keySet().iterator(); e.hasNext();) {
			Double elem = e.next();
			if (elem <= tm) {
				arl.add(elem);
			}
		}
		Double[] ar = arl.toArray(new Double[0]);
		Arrays.sort(ar);
		ar = Arrays.copyOfRange(ar, 0, ar.length);
		arl = new ArrayList<Double>(Arrays.asList(ar));
		return arl;
	}

	/** Return a list of aircraft names */
	public ArrayList<String> getNameIndex() {
		return new ArrayList<String>(nameIndex);
	}
	
	/** Returns true if an entry exists for the given name and time */
	public boolean hasEntry(String name, double time) {
		return sequenceTable.containsKey(time) && sequenceTable.get(time).containsKey(name);
	}
	

	public GeneralPlan getSequenceGeneralPlan(String name, double time) {
		if (sequenceTable.containsKey(time) && sequenceTable.get(time).containsKey(name)) {
			return sequenceTable.get(time).get(name);
		} else {
			error.addWarning("getSequenceGeneralPlan: invalid name/time combination");
			return new GeneralPlan();
		}
	}
	
	/** Returns a list of all Aircraft ids in the sequence */
	public ArrayList<String> getSequencePlanNameList() {
		return new ArrayList<String>(names);
	}

	/** sets a particular entry without reading in from a file */
	public void setEntry(double time, GeneralPlan p) {
		if (!sequenceTable.containsKey(time)) {
			sequenceTable.put(time, new HashMap<String, GeneralPlan >());
		}
		sequenceTable.get(time).put(p.getName(), p);
	}
	
	/**
	 * This purges all references of a given set of aircraft from this reader.
	 * This then resets the active time to the last time in the list
	 * @param alist List of aircraft identifiers
	 */
	public void removeGeneralPlan(ArrayList<String> alist) {
		for (String a : alist) {
			for (Double key : sequenceTable.keySet()) {
				if (sequenceTable.get(key).containsKey(a)) {
					sequenceTable.get(key).remove(a);
				}
			}
			if (names.contains(a)) {
				names.remove(a);
			}
			nameIndex.remove(a);
		}
		setLastActive();
	}

	public ArrayList<Pair<GeneralPlan,Double>> allEntries() {
		ArrayList<Pair<GeneralPlan,Double>> list = new ArrayList<Pair<GeneralPlan,Double>>();
		for (double t : sequenceTable.keySet()) {
			for (String name : sequenceTable.get(t).keySet()) {
				GeneralPlan p = sequenceTable.get(t).get(name);
				list.add(Pair.make(p, t));
			}
		}
		return list;
	}
	
	
	/**
	 * Return the name of the first aircraft that appears in the file, or null if there are no aircraft.
	 */
	public String defaultOwnship() {
		return defaultOwnship;
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
