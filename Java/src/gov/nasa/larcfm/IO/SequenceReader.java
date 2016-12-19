/* 
 *
 * Contact: George Hagen
 * 
 * Copyright (c) 2011-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

package gov.nasa.larcfm.IO;

import gov.nasa.larcfm.Util.AircraftState;
import gov.nasa.larcfm.Util.Constants;
import gov.nasa.larcfm.Util.ErrorLog;
import gov.nasa.larcfm.Util.LatLonAlt;
import gov.nasa.larcfm.Util.Pair;
import gov.nasa.larcfm.Util.Position;
import gov.nasa.larcfm.Util.Quad;
import gov.nasa.larcfm.Util.Util;
import gov.nasa.larcfm.Util.Vect3;
import gov.nasa.larcfm.Util.Velocity;

import java.io.*;
import java.util.ArrayList;
import java.util.List;
import java.util.Arrays;
import java.util.HashSet;
import java.util.Hashtable;
import java.util.Iterator;
import java.util.Map;
import java.util.Set;

/**
 * This reads in and stores a set of aircraft states, possibly over time, (and parameters) from a file
 * The Aircraft states are stored in an ArrayList&lt;AircraftState&gt;.<p>
 * 
 * This can read state/history files as well as sequence files (slight variations on history files).<p>
 *
 * State files consist of comma or space-separated values, with one point per line.
 * Required columns include aircraft name, 3 position columns (either x[nmi]/y[nmi]/z[ft] or latitude[deg]/longitude[deg]/altitude[ft]) and
 * 3 velocity columns (either vx[kn]/vy[kn]/vz[fpm] or track[deg]/gs[kn]/vs[fpm]).<p>
 *
 * An optional column is time [s].  If it is included, a "history" will be build if an aircraft has more than one entry.
 * If it is not included, only the last entry for an aircraft will be stored.<p>
 *
 * It is necessary to include a header line that defines the column ordering.  The column definitions are not case sensitive.
 * There is also an optional header line, immediately following the column definition, that defines the unit type for each
 * column (the defaults are listed above).<p>
 *
 * If points are consecutive for the same aircraft, subsequent name fields may be replaced with a double quotation mark (&quot;).
 * The aircraft name is case sensitive, so US54A != Us54a != us54a.<p>
 *
 * Any empty line or any line starting with a hash sign (#) is ignored.<p>
 *
 * Files may also include parameter definitions prior to other data.  Parameter definitions are of the form &lt;key&gt; = &lt;value&gt;,
 * one per line, where &lt;key&gt; is a case-insensitive alphanumeric word and &lt;value&gt; is either a numeral or string.  The &lt;value&gt;
 * may include a unit, such as <tt>dist = 50 [m]</tt>.  Note that parameters require a space on either side of the equals sign.
 * Note that it is possible to also update the stored parameter values (or store additional ones) through API calls.<p>
 *
 * Parameters can be interpreted as double values, strings, or Boolean values, and the user is required to know which parameter is
 * interpreted as which type.<p>
 * 
 * If the optional parameter <tt>filetype</tt> is specified, its value must be <tt>state</tt>, <tt>history</tt>, or <tt>sequence</tt> for this reader
 * to accept the file without error.<p>
 *
 */
public class SequenceReader extends StateReader {
 
	private Map< Double, Map<String, Pair<Position, Velocity> > > sequenceTable = new Hashtable< Double, Map<String, Pair<Position, Velocity> > >();
	private ArrayList<String> nameIndex = new ArrayList<String>();
	private Set<String> names = new HashSet<String>();
	private int windowSize = AircraftState.DEFAULT_BUFFER_SIZE;
	
	
    /** A new, empty StateReader.  This may be used to store parameters, but nothing else. */
	public SequenceReader() {
		error = new ErrorLog("SequenceReader(no file)");
		states = new ArrayList<AircraftState>(0);
		input = new SeparatedInput();
		input.setCaseSensitive(false);            // headers & parameters are lower case
		fname = "";
	}

	public SequenceReader(String filename) {
		fname = filename;
		error = new ErrorLog("SequenceReader("+filename+")");
		if (filename == null || filename.equals("")) {
			error.addError("No file specified");
			return;
		}
		SeparatedInput si;
		try {    
			si = new SeparatedInput(new FileReader(filename));
		} catch (FileNotFoundException e) {
			error.addError("File "+filename+" read protected or not found");
			return;
		}
        input = si;
		input.setCaseSensitive(false);            // headers & parameters are lower case
		loadfile();
	}

	/** Read a new file into an existing StateReader.  Parameters are preserved if they are not specified in the file. */
	public void readFile(String filename) {
		if (filename == null || filename.equals("")) {
			error.addError("No file specified");
			return;
		}
		fname = filename;
		SeparatedInput si;
		try {
			error = new ErrorLog("SequenceReader("+filename+")");
			si = new SeparatedInput(new FileReader(filename));
			si.setCaseSensitive(false);            // headers & parameters are lower case
			List<String> params = input.getParametersRef().getList();
			for (String p: params) {
				si.getParametersRef().set(p, input.getParametersRef().getString(p));
			}
			for (int i = 0; i < head.length; i++) {
				head[i] = -1;
			}
		} catch (FileNotFoundException e) {
			error.addError("File "+filename+" read protected or not found");
			if (sequenceTable != null) sequenceTable.clear();
			return;
		}
        input = si;
		loadfile();
	}
	
	
	private void loadfile() {
		hasRead = false;
		clock = true;
		//interpretUnits = false;
		sequenceTable = new Hashtable< Double, Map<String, Pair<Position, Velocity> > >();
		states = new ArrayList<AircraftState>();
		nameIndex = new ArrayList<String>();
		names = new HashSet<String>();
		String lastName = ""; // the current aircraft name
		//double lastTime = -1000000; // time must be increasing
		//int stateIndex = -1;

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

				head[NAME] =   altHeadings("name", "aircraft", "id");
				head[LAT_SX] = altHeadings("sx", "lat", "latitude");
				head[LON_SY] = altHeadings("sy", "lon", "long", "longitude");
				head[ALT_SZ] = altHeadings("sz", "alt", "altitude");
				head[TRK_VX] = altHeadings("trk", "vx", "track");
				head[GS_VY] = altHeadings("gs", "vy", "groundspeed", "groundspd");
				head[VS_VZ] = altHeadings("vs", "vz", "verticalspeed", "hdot");
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
                  if (!sval.equalsIgnoreCase("state") && !sval.equalsIgnoreCase("history") && !sval.equalsIgnoreCase("sequence")) {
            	    error.addError("Wrong filetype: "+sval);
            	    break;
                  }
              }

              hasRead = true;
              for (int i = 0; i <= VS_VZ; i++) {
            	  if (head[i] < 0) error.addError("This appears to be an invalid state file (missing header definitions)");
              }
			}

			String thisName = input.getColumnString(head[NAME]);
			
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

			Position ss;
			Velocity vv;
			double tm = 0.0;
			if (head[TM_CLK] >= 0) {
				tm = parseClockTime(input.getColumnString(head[TM_CLK]));
			}

			
			Map<String, Pair<Position, Velocity> >  sequenceEntry;
//f.pln("tm="+tm);
			if (sequenceTable.containsKey(tm)) {
				sequenceEntry = sequenceTable.get(tm);
//f.pln("old entry at "+tm);				
			} else {
				sequenceEntry = new Hashtable<String, Pair<Position, Velocity> >();
				sequenceTable.put(tm, sequenceEntry);
//f.pln("new entry at "+tm);				
//				lastTime = -1000000; // time must be increasing
//				stateIndex = -1;
			}

			if (input.hasError()) {
				error.addError(input.getMessage());
				sequenceTable.clear();
				break;
			}
			
			// the values are in the default units.
			if (latlon) {
				ss = new Position(LatLonAlt.mk(input.getColumn(head[LAT_SX], "deg"), 
							input.getColumn(head[LON_SY], "deg"), 
							input.getColumn(head[ALT_SZ], "ft")));
			} else {
				ss = new Position(new Vect3(
						input.getColumn(head[LAT_SX], "nmi"), 
						input.getColumn(head[LON_SY], "nmi"), 
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

//f.pln("$#%%# "+thisName+"  "+ss+"  "+vv+"  "+tm);			
			
			sequenceEntry.put(thisName, new Pair<Position,Velocity>(ss,vv));
			//lastTime = tm;
		}
        
        // reset accuracy parameters to their previous values
        Constants.set_horizontal_accuracy(h);
        Constants.set_vertical_accuracy(v);
        Constants.set_time_accuracy(t);
        
     
        // we initially load the LAST sequent as the active one
        setLastActive();
	}


	/** Return the number of sequence entries in the file */
	public int sequenceSize() {
		return sequenceTable.size();
	}
	
	/**
	 * Sets the window size for the active sequence set
	 * @param s > 0
	 */
	public void setWindowSize(int s) {
		if (s > 0) windowSize = s;
	}

	/**
	 * Returns the current window size
	 */
	public int getWindowSize() {
		return windowSize;
	}
	
	/** remove any time-point entries for which there is only one aircraft (and so no chance of conflict) */
	public void clearSingletons() {
		ArrayList<Double> keys = sequenceKeys();		
		for (int i = 0; i < keys.size(); i++) {
			if (sequenceTable.get(keys.get(i)).size() < 2) {
//f.pln("removing singleton (only one aircraft present) at time "+keys.get(i));				
				sequenceTable.remove(keys.get(i));
			}
		}
	}

	// we need to preserve the order of the aircraft as in the input file (because the first might be the only way we know which is the ownship)
	// so we build an arraylist states to us as the subset of all possible inputs
	private void buildActive(double tm) {
		ArrayList<Double> times = sequenceKeysUpTo(windowSize,tm); // Note: this includes the last entry
 		Map<String, Boolean> included = new Hashtable<String,Boolean>(10); // use to make sure there are no duplicates
		states = new ArrayList<AircraftState>(0);
		// build all AircraftStates that exist in these times
		for (int i = 0; i < nameIndex.size(); i++) { // work through the names in order
			String name = nameIndex.get(i);
			for (int j = 0; j < times.size(); j++) { // for each name, work through the times in the window
				double time = times.get(j);
				Map<String, Pair<Position, Velocity> > sequenceEntry = sequenceTable.get(time);
				if (sequenceEntry.containsKey(name)) {	// name has an entry at this time
					if (!included.containsKey(name)) {  // name has not been added to the states list yet
						// build a new AircraftState
						included.put(name, true);				// note name has been used
						states.add(new AircraftState(name)); 	// add new 
					}
					Pair<Position,Velocity> p = sequenceEntry.get(name);	// get entry info
					states.get(states.size()-1).add(p.first, p.second, time); // we always work with the last added states entry, because they're ordered by name
//f.pln("SequenceReader.buildActive "+p.first.toString()+" "+p.second.toString()+time);					
				}
			}
		}
	}
	
	/**
	 * Given a sequence key, set the active set of states.  If no such key exists, the active set is left empty.
	 * @param tm Sequence key (time)
	 */
	public void setActive(double tm) {
		states = new ArrayList<AircraftState>();
		if (sequenceTable.containsKey(tm)) {
			buildActive(tm);
		}
	}
	
	/**
	 * Set the first entry to be the active one.
	 */
	public void setFirstActive() {
		ArrayList<Double> keys = sequenceKeys();
		if (keys.size() > 0)
			buildActive(keys.get(0));
		else
			states = new ArrayList<AircraftState>();
	}

	/**
	 * Set the last entry to be the active one.
	 */
	public void setLastActive() {
		ArrayList<Double> keys = sequenceKeys();
		if (keys.size() > 0)
			buildActive(keys.get(keys.size()-1));
		else
			states = new ArrayList<AircraftState>();
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
	public ArrayList<Double> sequenceKeysUpTo(int n, double tm) {
		ArrayList<Double> arl = new ArrayList<Double>();
		for (Iterator<Double> e = sequenceTable.keySet().iterator(); e.hasNext();) {
			Double elem = e.next();
			if (elem <= tm) {
				arl.add(elem);
			}
		}
		Double[] ar = arl.toArray(new Double[0]);
		Arrays.sort(ar);
		ar = Arrays.copyOfRange(ar, Util.max(ar.length-n,0), ar.length);
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
	
	/** Returns the Position entry for a given name and time.  If no entry for this name and time, returns a zero position and sets a warning. */
	public Position getSequencePosition(String name, double time) {
		if (sequenceTable.containsKey(time) && sequenceTable.get(time).containsKey(name)) {
			return sequenceTable.get(time).get(name).first;
		} else {
			error.addWarning("getSequencePosition: invalid name/time combination");
			return Position.ZERO_LL;
		}
	}

	/** Returns the Velocity entry for a given name and time.  If no entry for this name and time, returns a zero velocity and sets a warning. */
	public Velocity getSequenceVelocity(String name, double time) {
		if (sequenceTable.containsKey(time) && sequenceTable.get(time).containsKey(name)) {
			return sequenceTable.get(time).get(name).second;
		} else {
			error.addWarning("getSequenceVelocity: invalid name/time combination");
			return Velocity.ZERO;
		}
	}
	
	/** Returns a list of all Aircraft ids in the sequence */
	public ArrayList<String> getSequenceAircraftIdList() {
		return new ArrayList<String>(names);
	}

	/** sets a particular entry without reading in from a file */
	public void setEntry(double time, String name, Position p, Velocity v) {
		if (!sequenceTable.containsKey(time)) {
			sequenceTable.put(time, new Hashtable<String, Pair<Position, Velocity> >());
		}
		sequenceTable.get(time).put(name, new Pair<Position,Velocity>(p,v));
	}
	
	/**
	 * This purges all references of a given set of aircraft from this reader.
	 * This then resets the active time to the last time in the list
	 * @param alist List of aircraft identifiers
	 */
	public void removeAircraft(ArrayList<String> alist) {
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

	public ArrayList<Quad<String,Position,Velocity,Double>> allEntries() {
		ArrayList<Quad<String,Position,Velocity,Double>> list = new ArrayList<Quad<String,Position,Velocity,Double>>();
		for (double t : sequenceTable.keySet()) {
			for (String name : sequenceTable.get(t).keySet()) {
				Pair<Position,Velocity> p = sequenceTable.get(t).get(name);
				list.add(Quad.make(name, p.first, p.second, t));
			}
		}
		return list;
	}
	
    public String toString() {
	//return input.toString();
    	String rtn = "SequenceReader: ------------------------------------------------\n";
    	ArrayList<Double> keys = sequenceKeys();
    	if (keys.size() > 0) {
    	  rtn = rtn+" Sequence start: "+keys.get(0)+"\n";
    	  rtn = rtn+" Sequence end: "+keys.get(keys.size()-1)+"\n";
    	  rtn = rtn + "Size: "+keys.size()+"\n";
    	} else {
    		rtn = rtn+" Empty sequence\n";
    	}
    	rtn = rtn+" ACTIVE:\n";
    	for (int j = 0; j < states.size(); j++) {
            rtn = rtn + states.get(j)+ "\n";
    	}
    	rtn = rtn+input.toString();
    	return rtn;
    }

}
