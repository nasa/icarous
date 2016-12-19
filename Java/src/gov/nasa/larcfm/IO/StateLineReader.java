/*
 * Copyright (c) 2014-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
package gov.nasa.larcfm.IO;

import gov.nasa.larcfm.Util.LatLonAlt;
import gov.nasa.larcfm.Util.ParameterData;
import gov.nasa.larcfm.Util.Point;
import gov.nasa.larcfm.Util.Position;
import gov.nasa.larcfm.Util.Quad;
import gov.nasa.larcfm.Util.Constants;
import gov.nasa.larcfm.Util.Velocity;
import gov.nasa.larcfm.Util.ErrorReporter;
import gov.nasa.larcfm.Util.ErrorLog;

import java.io.BufferedReader;
import java.io.Reader;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;
import java.io.Closeable;
import java.util.ArrayList;
import java.util.List;

public class StateLineReader implements StateBlockReader, Closeable {
	private SeparatedInput input; // = new SeparatedInput();

	private int[] head = { -1, -2, -3, -4, -5, -6, -7, -8 }; // array size of  number of
															 // elements (vector in C++)
	private boolean latlon;
	private boolean velocity;
	private boolean trkgsvs;
	private boolean first_line;
	private String fname;
	private ErrorLog error;
	private Reader fr;
	
	private Quad<String,Position,Velocity,Double> line;

	private final int NAME = 0;
	private final int LAT_SX = 1;
	private final int LON_SY = 2;
	private final int ALT_SZ = 3;
	private final int TRK_VX = 4;
	private final int GS_VY = 5;
	private final int VS_VZ = 6;
	private final int TM_CLK = 7;
	
	public StateLineReader() {
		input = new SeparatedInput();
		error = new ErrorLog("StateLineReader()");
		fname = "<none>";
		line = null;
	}

	/**
	 * Read a new file into an existing StateReader. Parameters are preserved if
	 * they are not specified in the file.
	 */
	public void open(String filename) {
		if (filename == null || filename.equals("")) {
			error.addError("No file specified");
			return;
		}
		close();
		fname = filename;
		try {
			fr = new BufferedReader(new FileReader(filename));
			open(fr);
		} catch (FileNotFoundException e) {
			error.addError("File " + filename + " read protected or not found in "+System.getProperty("user.dir"));
			return;
		}
	}

	/**
	 * Read a new file into an existing StateReader. Parameters are preserved if
	 * they are not specified in the file.
	 */
	public void open(Reader r) {
		//f.pln(" $$$$$$$$$$$$$$$$$$$$$ StateLineReader: open r = "+r);
		if (r == null) {
			error.addError("Null Reader provided");
			return;
		}
		first_line = true;
		line = null;
		SeparatedInput si;
		si = new SeparatedInput(r);
		si.setCaseSensitive(false); // headers & parameters are lower case
		List<String> params = getParametersRef().getList();
		for (String p: params) {
			si.getParametersRef().set(p, getParametersRef().getString(p));
		}
		for (int i = 0; i < head.length; i++) {
			head[i] = -1;
		}
		input = si;
	}

	/** Close this StateLineReader */
    public void close() {
    	if (fr != null) {
    		try {
    			fr.close();
    		} catch (IOException e) {
    			error.addError("IO Exception in close(): "+e.getMessage());
    		}
    		fname = "<none>";
        	fr = null;
    	}
    }
    
	private void readFirstLine() {
		latlon = input.findHeading(Constants.LAT_OR_LON_HEADINGS) >= 0;
		head[NAME] = input.findHeading(Constants.NAME_HEADINGS);
		head[LAT_SX] = input.findHeading(Constants.LATITUDE_HEADINGS);
		head[LON_SY] = input.findHeading(Constants.LONGITUDE_HEADINGS);
		head[ALT_SZ] = input.findHeading(Constants.ALTITUDE_HEADINGS);
		head[TRK_VX] = input.findHeading("trk", "vx", "track");
		head[GS_VY] = input.findHeading("gs", "vy", "groundspeed", "groundspd");
		head[VS_VZ] = input.findHeading("vs", "vz", "verticalspeed", "hdot");
		head[TM_CLK] = input.findHeading(Constants.TIME_HEADINGS);
		trkgsvs = input.findHeading("trk", "track") >= 0;
		velocity = head[TRK_VX] >= 0 && head[GS_VY] >= 0 && head[VS_VZ] >= 0;
		line = null;
	}

	/** 
	 * Returns one line from the file
	 * @return a datastructure holding the name of the aircraft, its position, its velocity, and the time. A null is returned if there is no more data
	 */
	public Quad<String, Position, Velocity, Double> readLine() {
		if (line != null) {
			Quad<String, Position, Velocity, Double> temp = line;
			line = null;
			return temp;
		}
		
		// look for each possible heading
		if (input.readLine())
			return null; // end-of-file
		
		if (first_line) {
			readFirstLine();
			first_line = false;
		}

		String acName = input.getColumnString(head[NAME]);
		double time = -1.0;
		if (head[TM_CLK] >= 0) {
			time = input.getColumn(head[TM_CLK]);
		}
		Position pos;
		if (latlon) {
			pos = new Position(LatLonAlt.mk(
					input.getColumn(head[LAT_SX], "deg"),
					input.getColumn(head[LON_SY], "deg"),
					input.getColumn(head[ALT_SZ], "ft")));
		} else {
			pos = new Position(Point.mk(input.getColumn(head[LAT_SX], "nmi"),
					input.getColumn(head[LON_SY], "nmi"),
					input.getColumn(head[ALT_SZ], "ft")));
		}
		Velocity vel;
		if (velocity) {
			if (trkgsvs) {
				vel = Velocity.mkTrkGsVs(input.getColumn(head[TRK_VX], "deg"),
						input.getColumn(head[GS_VY], "knot"),
						input.getColumn(head[VS_VZ], "fpm"));
			} else {
				vel = Velocity.mkVxyz(input.getColumn(head[TRK_VX], "knot"),
						input.getColumn(head[GS_VY], "knot"),
						input.getColumn(head[VS_VZ], "fpm"));
			}
		} else {
			vel = Velocity.INVALID;
		}

		// f.pln(simTime+" acName = "+acName+" pos = "+pos+" vel = "+vel);

		return new Quad<String, Position, Velocity, Double>(acName, pos, vel,
				time);

	}
	
	/** 
	 * Reads a sequence of lines that all have the same time and returns these lines as an ArrayList
	 * @return a list of datastructure holding the name of the aircraft, its position, its velocity, and the time.
	 */
	public ArrayList<Quad<String, Position, Velocity, Double>> readTimeBlock() {
		ArrayList<Quad<String, Position, Velocity, Double>> list = new ArrayList<Quad<String, Position, Velocity, Double>>(20);
		Quad<String, Position, Velocity, Double> first = null;
		
		while(true) {
			Quad<String, Position, Velocity, Double> temp = readLine();
			if (temp == null) {
				break;
			}
			if (first == null) {
				first = temp;
			}
			if (! temp.fourth.equals(first.fourth)) {
				line = temp;
				break;
			}
			list.add(temp);
		}
		return list;		
	}
	
    public String getFilename() {
    	return fname;
    }   
	
	public boolean isVelocity() {
		return velocity;
	}

	public boolean isLatlon() {
		return latlon;
	}

	public boolean isTrkGsVs() {
		return trkgsvs;
	}
	
	//
	// ParameterReader methods
	//

	public ParameterData getParametersRef() {
		return input.getParametersRef();
	}

	@Override
	public ParameterData getParameters() {
		return new ParameterData(input.getParametersRef());
	}
	
	public void updateParameterData(ParameterData p) {
		p.copy(input.getParametersRef(), true);
	}

	//
	// ErrorReporter Interface Methods
	//

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



}
