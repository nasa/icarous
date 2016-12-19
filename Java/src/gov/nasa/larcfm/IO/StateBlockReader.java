/*
 * Copyright (c) 2015-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
package gov.nasa.larcfm.IO;

import gov.nasa.larcfm.Util.ErrorReporter;
import gov.nasa.larcfm.Util.ParameterProvider;
import gov.nasa.larcfm.Util.ParameterReader;
import gov.nasa.larcfm.Util.Position;
import gov.nasa.larcfm.Util.Quad;
import gov.nasa.larcfm.Util.Velocity;

import java.io.Closeable;
import java.util.ArrayList;

public interface StateBlockReader extends ParameterReader, ParameterProvider, ErrorReporter, Closeable  {

	public void open(String filename);
    public void close();
	public ArrayList<Quad<String, Position, Velocity, Double>> readTimeBlock();

}
