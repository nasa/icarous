/* 
 * ATOSNavDatabase
 *
 * Contact: Jeff Maddalon
 *
 * Copyright (c) 2016-2017 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

package gov.nasa.larcfm.IO;

import gov.nasa.larcfm.Util.BoundingRectangle;
import gov.nasa.larcfm.Util.ErrorLog;
import gov.nasa.larcfm.Util.ErrorReporter;
import gov.nasa.larcfm.Util.GreatCircle;
import gov.nasa.larcfm.Util.LatLonAlt;
import gov.nasa.larcfm.Util.ParameterData;
import gov.nasa.larcfm.Util.ParameterProvider;
import gov.nasa.larcfm.Util.Quad;
import gov.nasa.larcfm.Util.Triple;
import gov.nasa.larcfm.Util.Units;
import gov.nasa.larcfm.Util.Util;

import java.io.*;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

/**
 * A database of various Navigation information.  Uses the file format
 * from the Air Traffic Operation Simulation (ATOS)
 * 
 */
public class ATOSNavDatabase implements ParameterProvider, ErrorReporter {
	protected ErrorLog error;
	protected HashMap<String,Triple<String,LatLonAlt,String>> fix_db;
	protected HashMap<String,ArrayList<String>> aw_db;
	protected HashMap<String,Triple<String,LatLonAlt,String>> ap_db;
	protected HashMap<String,HashMap<String, HashMap<String, Quad<String, ArrayList<String>, ArrayList<String>, ArrayList<String>>>>> stars_db;
	protected HashMap<String,HashMap<String, HashMap<String, Quad<String, ArrayList<String>, ArrayList<String>, ArrayList<String>>>>> sids_db;
	protected ParameterData p;
	protected int[] head = new int[20]; // array size of number of elements (vector in C++)
	// we store the heading indices in the following order:
	protected final int NAME = 0;
	protected final int LAT = 1;
	protected final int LON = 2;
	protected final int TYPE = 3;
	protected final int OTHER = 4;
	protected final int AIRWAY = 5;
	protected final int FIX = 6;
	protected final int ID = 7;
	protected final int ELEV = 8;
	protected final int MAGVAR = 9;
	private String stars_fn = "Stars-file";
	private String sids_fn = "Sids-file";
	

	public ATOSNavDatabase() {
		error = new ErrorLog("ATOSNavDatabase()");
		fix_db = new HashMap<String,Triple<String,LatLonAlt,String>>(1000);
		aw_db = new HashMap<String,ArrayList<String>>(100);
		ap_db = new HashMap<String,Triple<String,LatLonAlt,String>>(100);
		stars_db = new HashMap<String,HashMap<String, HashMap<String, Quad<String, ArrayList<String>, ArrayList<String>, ArrayList<String>>>>>(100);
		sids_db = new HashMap<String,HashMap<String, HashMap<String, Quad<String, ArrayList<String>, ArrayList<String>, ArrayList<String>>>>>(100);
		p = new ParameterData();
	}

	/** Create a new PlanReader reading the specified file. 
	 * 
	 * @param fix_filename   database of navigation fixes and navigation aids
	 * @param aw_filename    airway database
	 * @param ap_filename    filename of airport database
	 * @param sids_filename  filename of SIDS database
	 * @param stars_filename filename of STARS database
	 */
	public void open(String fix_filename, String aw_filename, String ap_filename, String sids_filename, String stars_filename) {
		error = new ErrorLog("ATOSNavDatabase("+fix_filename+")");
		FileReader fix_fr = get_file_reader(fix_filename);
		FileReader aw_fr = get_file_reader(aw_filename);
		FileReader ap_fr = get_file_reader(ap_filename);
		FileReader stars_fr = get_file_reader(stars_filename);
		FileReader sids_fr = get_file_reader(sids_filename);
		
		stars_fn = stars_filename;
		sids_fn = sids_filename;
		
		open(fix_fr, aw_fr, ap_fr, stars_fr, sids_fr);
	}

	public void open(Reader fix_r, Reader aw_r, Reader ap_r, Reader sids_r, Reader stars_r)  {
		SeparatedInput input;
		if (fix_r != null) {
			input = new SeparatedInput(fix_r);
			input.setCaseSensitive(false);            // headers & parameters are lower case
			input.setFixedColumn("5,7,13,15,19","Name,Type,Lat,Lon,Other","unitless,unitless,unitless,unitless,unitless");
			loadfile_fix(input);
			p.copy(input.getParametersRef(), true);
		}

		if (aw_r != null) {
			input = new SeparatedInput(aw_r);
			input.setCaseSensitive(false);            // headers & parameters are lower case
			input.setFixedColumn("4,5","Airway,Fix","unitless,unitless");
			loadfile_aw(input);
			p.copy(input.getParametersRef(), true);
		}
		
		if (ap_r != null) {
			input = new SeparatedInput(ap_r);
			input.setCaseSensitive(false);            // headers & parameters are lower case
			input.setFixedColumn("11,4,43,14,14,5,3","Misc,Id,Name,Lat,Lon,elev,MagVar","unitless,unitless,unitless,unitless,unitless,unitless,unitless");
			loadfile_ap(input);
			p.copy(input.getParametersRef(), true);
		}
		
		if (stars_r != null) {
			loadfile_stars(stars_fn, new LineNumberReader(stars_r));
		}
		
		if (sids_r != null) {
			loadfile_sids(sids_fn, new LineNumberReader(sids_r));
		}
		
		integrity_check();
	}

	private FileReader get_file_reader(String filename) {
		if (filename == null || filename.equals("")) {
			error.addError("Empty file name");			
			return null;
		}
		FileReader fr;
		try {
			fr = new FileReader(filename);
		} catch (FileNotFoundException e) {
			error.addError("File "+filename+" read protected or not found");
			if (fix_db != null) fix_db.clear();
			return null;
		}
		
		return fr;
	}

	private void integrity_check() {
		//TODO: check things like, that all fixes in a airway are in the fix database, what
		// to do if they aren't? throw an error???
		
		//TODO: check if airport in SIDS/STARS is in airport database
		
		//TODO: check if navfix in SIDS/STARS is in navfix database
	}
	
	
	private void loadfile_fix(SeparatedInput input) {
		boolean hasReadOnce = false;

		while ( ! input.readLine()) {
			// look for each possible heading
			if ( ! hasReadOnce) {
				head[NAME] = 	input.findHeading("name");
				head[LAT] = 	input.findHeading("lat");
				head[LON] = 	input.findHeading("long","lon");
				head[TYPE] = 	input.findHeading("type");
				head[OTHER] =	input.findHeading("other");

				hasReadOnce = true;
			}
			
			if (input.hasError()) {
				error.addError(input.getMessage());
				break;
			}
			
			
			String name = input.getColumnString(head[NAME]).trim();

			LatLonAlt lla;
			lla = LatLonAlt.mk(
					GreatCircle.parse_degrees2(input.getColumnString(head[LAT])), 
					GreatCircle.parse_degrees2(input.getColumnString(head[LON])), 
					0.0);
			
			String type = input.getColumnString(head[TYPE]).trim();
			String other = input.getColumnString(head[OTHER]).trim();   // apparently this has the frequency of the point
			
			fix_db.put(name, Triple.make(type,lla,other));
		}
	}


	private void loadfile_aw(SeparatedInput input) {
		boolean hasReadOnce = false;

		while ( ! input.readLine()) {
			// look for each possible heading
			if ( ! hasReadOnce) {
				head[AIRWAY] = 	input.findHeading("Airway");
				head[FIX] = 	input.findHeading("Fix");

				hasReadOnce = true;
			}
			
			if (input.hasError()) {
				error.addError(input.getMessage());
				break;
			}
			
			
			String name = input.getColumnString(head[AIRWAY]).trim(); // this assumes that all airways are four characters J001, never J1
			String fix = input.getColumnString(head[FIX]).trim();

			if (aw_db.containsKey(name)) {
				ArrayList<String> list = aw_db.get(name);
				list.add(fix);
			} else {
				aw_db.put(name, new ArrayList<String>(Arrays.asList(fix)));
			}
		}
	}


	private void loadfile_ap(SeparatedInput input) {
		boolean hasReadOnce = false;

		while ( ! input.readLine()) {
			// look for each possible heading
			if ( ! hasReadOnce) {
				head[ID] = 		input.findHeading("id");
				//head[LAT] = 	input.findHeading("id2");  // ignore this column  
				head[NAME] = 	input.findHeading("name");
				head[LAT] = 	input.findHeading("lat");
				head[LON] = 	input.findHeading("long","lon");
				head[ELEV] = 	input.findHeading("elev");
				head[MAGVAR] =	input.findHeading("magvar");
				hasReadOnce = true;
			}
			
			if (input.hasError()) {
				error.addError(input.getMessage());
				break;
			}
			
			
			String id = input.getColumnString(head[ID]).trim();
			String fullname = input.getColumnString(head[NAME]).trim();

			LatLonAlt lla;
			lla = LatLonAlt.mk(
					GreatCircle.parse_degrees2(input.getColumnString(head[LAT])), 
					GreatCircle.parse_degrees2(input.getColumnString(head[LON])),
					Units.parse("ft",input.getColumnString(head[ELEV])));
			
			String magvar = input.getColumnString(head[MAGVAR]).trim();   // apparently this has the frequency of the point
			
			ap_db.put(id, Triple.make(fullname, lla, magvar));
		}
	}

	private void loadfile_stars(String filename, LineNumberReader reader) {
		boolean old_format = false;
		int starter;
		String str = null;
		try {
			while ((str = reader.readLine()) != null) {
				str = str.trim();
				// Skip empty lines or lines starts with #
				if (str.length() == 0 || str.charAt(0) == '#') {
					continue;
				}
				
				if (str.equals("CIFP")) {
					old_format = true;
					continue;
				}
				
				String[] comp = str.split(":",128);  // 128 is arbitrary, this is enough for 40 route points, the longest route I could find was 13 points
				comp = Arrays.copyOfRange(comp,0,comp.length-1);

				String airportName = comp[0];
				String procedureName = comp[1].trim();
				String routeType;
				String transistionName;
				if (old_format) {
					routeType = comp[2];
					transistionName = comp[3].trim();
					starter = 4;
				} else {
					routeType = "X";
					transistionName = comp[2];
					starter = 2;
				}
				ArrayList<String> routeFix = new ArrayList<String>(4);
				ArrayList<String> routeAlt = new ArrayList<String>(4);
				ArrayList<String> routeSpeed = new ArrayList<String>(4);

				for (int i = starter; i < comp.length; i = i + 3) {
					if ( i+2 >= comp.length) {
						error.addWarning("In file "+filename+", line "+reader.getLineNumber()
								+ " had an unexpected number of columns, skipping.");
						continue;
					} 
					//f.pln(" $$$$ airport "+airportName+" procedure "+procedureName+" transistion "+transistionName+" route point: "+comp[i]+" - "+comp[i+1]+" - "+comp[i+2]);
					routeFix.add(comp[i]);
					routeAlt.add(comp[i+1]);
					routeSpeed.add(comp[i+2]);
				}

				HashMap<String, HashMap<String, Quad<String, ArrayList<String>, ArrayList<String>, ArrayList<String>>>> airport_star;			
				if (stars_db.containsKey(airportName)) {
					airport_star = stars_db.get(airportName);
				} else {
					airport_star = new HashMap<String, HashMap<String, Quad<String, ArrayList<String>, ArrayList<String>, ArrayList<String>>>>(3);
					stars_db.put(airportName, airport_star);
				}
				
				HashMap<String, Quad<String, ArrayList<String>, ArrayList<String>, ArrayList<String>>> procedure;
				if (airport_star.containsKey(procedureName)) {
					procedure = airport_star.get(procedureName);
				} else {
					procedure = new HashMap<String, Quad<String, ArrayList<String>, ArrayList<String>, ArrayList<String>>>(3);
					airport_star.put(procedureName, procedure);
				}
				
				Quad<String, ArrayList<String>, ArrayList<String>, ArrayList<String>> transistion;
				transistion = Quad.make(routeType, routeFix, routeAlt, routeSpeed);
				procedure.put(transistionName, transistion);
				//f.pln("Airport star: "+airport_star);
			}//while    
		} catch (IOException e) { 
			error.addError("*** In file "+filename+", an IO error occured at line "+reader.getLineNumber()
					+ "The error was:"+e.getMessage());
		}
	}


	private void loadfile_sids(String filename, LineNumberReader reader) {
		String str = null;
		boolean new_format = false;
		int starter;
		try {
			while ((str = reader.readLine()) != null) {
				str = str.trim();
				// Skip empty lines or lines starts with #
				if (str.length() == 0 || str.charAt(0) == '#') {
					continue;
				}
				if (str.equals("CIFP")) continue;

				String[] comp = str.split(":",64);  // 64 is arbitrary, this is enough for 20 route point, the longest route I could find was 13 points
				comp = Arrays.copyOfRange(comp,0,comp.length-1);
				//f.pln("comp len "+comp.length);
				if (comp.length % 3 == 0) {
					new_format = true;
				}

				String airportName = comp[0];
				String procedureName = comp[1].trim();
				String routeType;
				String transistionName;
				if (new_format) {
					routeType = "X";
					transistionName = comp[2].trim();
					starter = 3;
				} else {
					routeType = comp[2];
					transistionName = comp[3].trim();
					starter = 4;
				}
				ArrayList<String> routeFix = new ArrayList<String>(4);
				ArrayList<String> routeAlt = new ArrayList<String>(4);
				ArrayList<String> routeSpeed = new ArrayList<String>(4);

				for (int i = starter; i < comp.length; i = i + 3) {
					if ( i+2 >= comp.length) {
						error.addWarning("In file "+filename+", line "+reader.getLineNumber()
								+ " had an unexpected number of columns, skipping.");
						continue;
					}
					routeFix.add(comp[i]);
					routeAlt.add(comp[i+1]);
					routeSpeed.add(comp[i+2]);
				}

				if (new_format) {
					transistionName = routeFix.get(routeFix.size()-1);
				}
				HashMap<String, HashMap<String, Quad<String, ArrayList<String>, ArrayList<String>, ArrayList<String>>>> airport_sid;
				
				if (sids_db.containsKey(airportName)) {
					airport_sid = sids_db.get(airportName);
				} else {
					airport_sid = new HashMap<String, HashMap<String, Quad<String, ArrayList<String>, ArrayList<String>, ArrayList<String>>>>(3);
					sids_db.put(airportName, airport_sid);
				}
				
				HashMap<String, Quad<String, ArrayList<String>, ArrayList<String>, ArrayList<String>>> procedure;
				if (airport_sid.containsKey(procedureName)) {
					procedure = airport_sid.get(procedureName);
				} else {
					procedure = new HashMap<String, Quad<String, ArrayList<String>, ArrayList<String>, ArrayList<String>>>(3);
					airport_sid.put(procedureName, procedure);
				}
				
				Quad<String, ArrayList<String>, ArrayList<String>, ArrayList<String>> transistion;
				transistion = Quad.make(routeType, routeFix, routeAlt, routeSpeed);
				procedure.put(transistionName, transistion);			
			}//while    
		} catch (IOException e) { 
			error.addError("*** In file "+filename+", an IO error occured at line "+reader.getLineNumber()
					+ "The error was:"+e.getMessage());
		}
	}
	
	/** Return the number of plans in the file 
	 * @return size
	 * */
	public int size() {
		return fix_db.size();
	}

	private Triple<String,LatLonAlt,String> getData(String name) {
		return fix_db.get(name);
	}
	
	public boolean containsFix(String name) {
		Triple<String,LatLonAlt,String> t = getData(name);
		if (t == null) return false;
		return true;
	}
	
	public LatLonAlt getFix(String name) {
		Triple<String,LatLonAlt,String> t = getData(name);
		if (t == null) return LatLonAlt.INVALID;
		return t.second;
	}

	public String getFixType(String name) {
		Triple<String,LatLonAlt,String> t = getData(name);
		if (t == null) return "not found";
		return t.first;
	}

	public String getFixOther(String name) {
		Triple<String,LatLonAlt,String> t = getData(name);
		if (t == null) return "not found";
		return t.third;
	}

	public ArrayList<String> getAirway(String name) {
		ArrayList<String> t;

		// Check for abbreviated form
		String alt_name = name;
		if (name.length() == 2) {
			alt_name = "" + name.charAt(0) + "00" + name.charAt(1);
		} else if (name.length() == 3) {
			alt_name = "" + name.charAt(0) + "0" + name.substring(1,3);
		} 
		
		t = aw_db.get(alt_name);
		if (t == null) {
			return new ArrayList<String>(0);
		}
		return t;
	}
	
	/**
	 * Returns information about the given airport id.
	 * @param id
	 * @return Airport's full_name, Location and elevation, Magnetic Variation
	 */
	public Triple<String,LatLonAlt,String> getAirport(String id) {
		Triple<String,LatLonAlt,String> t;

		if (id.length() == 3) {
			id = "K"+id;
		}
		
		t = ap_db.get(id);
		if (t == null) {
			return Triple.make("Airport "+id+" not found",LatLonAlt.INVALID,"not found");
		}
		return t;
	}

	/**
	 * Return a map for a standard arrival route procedure at an airport.<p>
	 * 
	 * Route Type - has ordinal values 1-6 taken from the CIFP data file where:<p>
	 * 1. Indicates a STAR enroute transition<br>
	 * 2. Indicates a STAR common route<br>
	 * 3. Indicates a STAR runway transition<br>
	 * 4. Indicates a RNAV STAR enroute transition<br>
	 * 5. Indicates a RNAV STAR common route<br>
	 * 6. Indicates a RNAV STAR runway transition<br>
	 * @param airport
	 * @param procedureName
	 * @return A map, mapping a route to a 4-Tuple of route type (see description above), Array of fixes, Array of altitude constraints, Array of speed constraints
	 */
	public HashMap<String, Quad<String, ArrayList<String>, ArrayList<String>, ArrayList<String>>> getStarTransition(String airport, String procedureName) {
		HashMap<String, HashMap<String, Quad<String, ArrayList<String>, ArrayList<String>, ArrayList<String>>>> airport_star;
		if (airport.length() == 3) {
			airport = "K"+airport;
		}
		
		airport_star = stars_db.get(airport);
		if (airport_star == null) return null;
		
		HashMap<String, Quad<String, ArrayList<String>, ArrayList<String>, ArrayList<String>>> procedure;
		procedure = airport_star.get(procedureName);
		
		return procedure;
	}

	public HashMap<String, Quad<String, ArrayList<String>, ArrayList<String>, ArrayList<String>>> getSidTransition(String airport, String procedureName) {
		HashMap<String, HashMap<String, Quad<String, ArrayList<String>, ArrayList<String>, ArrayList<String>>>> airport_sid;
		if (airport.length() == 3) {
			airport = "K"+airport;
		}
		airport_sid = sids_db.get(airport);
		if (airport_sid == null) return null;
		
		HashMap<String, Quad<String, ArrayList<String>, ArrayList<String>, ArrayList<String>>> procedure;
		procedure = airport_sid.get(procedureName);
		
		return procedure;
	}

	// ParameterReader methods

	public ParameterData getParameters() {
		return new ParameterData(p);
	}

	public void updateParameterData(ParameterData pd) {
		pd.copy(p, true);
	}
	
	public static boolean airportNameEquals(String name1, String name2) {
		if (name1.equals(name2)) return true;
		if (name1.length()==3) {
			name1 = "K"+name1;
		}
		if (name2.length()==3) {
			name2 = "K"+name1;
		}
		return name1.equals(name2);
	}

	/**
	 * Return the airport code that is closest horizontally to the given point, or the empty string if there are none.
	 * This will be a slow search.
	 * @param p position
	 * @return closest airport
	 */
	public String closestAirport(LatLonAlt p) {
		double dist = Double.MAX_VALUE;
		String s = "";
		for (Map.Entry<String, Triple<String, LatLonAlt, String>> o: ap_db.entrySet()) {
			String id = o.getKey();
			double d = GreatCircle.distance(p,o.getValue().second);
			if (d < dist) {
				dist = d;
				s = id;
			}
		}
		return s;
	}


	/**
	 * Return the fix that is closest horizontally to the given point, or the empty string if there are none.
	 * This will be a slow search.
	 * @param p position
	 * @return fix
	 */
	public String closestFix(LatLonAlt p) {
		double dist = Double.MAX_VALUE;
		String s = "";
		for (Map.Entry<String, Triple<String, LatLonAlt, String>> o: fix_db.entrySet()) {
			String id = o.getKey();
			double d = GreatCircle.distance(p,o.getValue().second);
			if (d < dist) {
				dist = d;
				s = id;
			}
		}
		return s;
	}

	/**
	 * Return a list of fixes that is with "dist" (horizontally) to the given point, or an empty list if there are none.
	 * 
	 * @param p 
	 * @param dist
	 * @return a list of fixes
	 */
	public List<String> findNearestNamedFixes(LatLonAlt p, double dist) {
		ArrayList<String> l = new ArrayList<String>(10);
		for (Map.Entry<String, Triple<String, LatLonAlt, String>> o: fix_db.entrySet()) {
			String id = o.getKey();
			if (Util.is_double(id)) continue;
			double d = GreatCircle.distance(p,o.getValue().second);
			if (d < dist) {
				l.add(id);
			}
		}
		return l;
	}

	/**
	 * Insert sort for findNearestNFixes()
	 * @param s fix to insert
	 * @param l current list
	 * @param n max list size
	 */
	private void insertSort(String s, ArrayList<String> l, LatLonAlt p, int n) {
		double d1 = GreatCircle.distance(p,getFix(s));
		boolean added = false;
		for (int i = 0; i < n && i < l.size(); i++) {
			String s2 = l.get(i);
			double d2 = GreatCircle.distance(p,getFix(s2));
			if (d1 < d2) {
				l.add(i,s);
				if (l.size() > n) {
					l.remove(n);
				}
				added = true;
				break;
			}
		}
		// add to end of list
		if (!added && l.size() < n) {
			l.add(s);
		}
	}

	/**
	 * Return a list of the (up to) n fixes that are closest (horizontally) to the given point, or an empty list if there are none.
	 * This list is ordered by distance from the given point (closest first).
	 * 
	 * @param p 
	 * @param n
	 * @return list of fixes
	 */
	public List<String> findNearestNNamedFixes(LatLonAlt p, int n) {
		ArrayList<String> l = new ArrayList<String>(n);
		for (Map.Entry<String, Triple<String, LatLonAlt, String>> o: fix_db.entrySet()) {
			String id = o.getKey();
			if (Util.is_double(id)) continue;
			insertSort(id, l, p, n);
		}
		return l;
	}

	/**
	 * Return the named fix that is closest horizontally to the given point, or the empty string if there are none.
	 * This will be a slow search.
	 * @param p position
	 * @return named fix
	 */
	public String closestNamedFix(LatLonAlt p) {
		double dist = Double.MAX_VALUE;
		String s = "";
		for (Map.Entry<String, Triple<String, LatLonAlt, String>> o: fix_db.entrySet()) {
			String id = o.getKey();
			if (Util.is_double(id)) continue;
			LatLonAlt fix = o.getValue().getSecond(); 
			double d = GreatCircle.distance(p,fix);
			if (d < dist) {
				dist = d;
				s = id;
			}
		}	
		return s;
	}
	
	/**
	 * Return the closest airport or fix (both named and unnamed) to the given point, or an empty string if there are none.
	 * This will be a slow search.
	 * @param p position
	 * @return airport
	 */
	public String closestPoint(LatLonAlt p) {
		String s1 = closestAirport(p);
		String s2 = closestFix(p);
		if (s1.length() > 0 && s2.length() > 0) {
			if (GreatCircle.distance(p, getAirport(s1).second) < GreatCircle.distance(p, getFix(s2))) {
				return s1;
			} else {
				return s2;
			}
		} else if (s1.length() > 0) {
			return s1;
		} else if (s2.length() > 0) {
			return s2;
		} else {
			return "";
		}
	}
	
	public ArrayList<String> airportList() {
		return new ArrayList<String>(ap_db.keySet());
	}

	public ArrayList<String> fixList() {
		return new ArrayList<String>(fix_db.keySet());
	}

	/**
	 * Return a list of named fixes within d of the indicated position, if any
	 * @param p target position
	 * @param d diatance (in m)
	 * @return list of named fixes within d of p
	 */
	public ArrayList<String> namedFixesWithinRange(LatLonAlt p, double d) {
		ArrayList<String> slist = new ArrayList<String>();
		for (Map.Entry<String, Triple<String, LatLonAlt, String>> o: fix_db.entrySet()) {
			String id = o.getKey();
			if (Util.is_double(id)) continue;
			LatLonAlt fix = o.getValue().getSecond(); 
			double dd = GreatCircle.distance(p,fix);
			if (dd <= d) {
				slist.add(id);
			}
		}	
		return slist;
	}
	
	/**
	 * Return a subset of a larger database, restricted by the given bounds
	 * @param rect geographic boundaries
	 * @return a database limited by geographic boundaries
	 */
	public ATOSNavDatabase subrange(BoundingRectangle rect) {
		ATOSNavDatabase db = new ATOSNavDatabase();
		p = db.getParameters();
		db.aw_db = aw_db;
		db.stars_db = stars_db;
		db.sids_db = sids_db;
		for (String key : fix_db.keySet()) {
			if (rect.contains(fix_db.get(key).second)) {
				db.fix_db.put(key, fix_db.get(key));
			}
		}
		for (String key : ap_db.keySet()) {
			if (rect.contains(ap_db.get(key).second)) {
				db.ap_db.put(key, ap_db.get(key));
			}
		}
		return db;
	}
	
	// ErrorReporter Interface Methods

	public boolean hasError() {
		return error.hasError();
	}
	public boolean hasMessage() {
		return error.hasMessage();
	}
	public String getMessage() {
		return error.getMessage();
	}
	public String getMessageNoClear() {
		return error.getMessageNoClear();
	}

//	public String toString() {
//		return input.toString();
//	}


}
