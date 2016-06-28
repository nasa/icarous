///*
// * Copyright (c) 2011-2016 United States Government as represented by
// * the National Aeronautics and Space Administration.  No copyright
// * is claimed in the United States under Title 17, U.S.Code. All Other
// * Rights Reserved.
// */
//
//package gov.nasa.larc.ICAROUS;
//
//
//import gov.nasa.larcfm.Util.*;
//
//// Class to hold basic aircraft information.
//
//public class Aircraft implements ErrorReporter {
//
//	// Parameters of the aircraft
//	private String id_; // Identifier
//	protected Position s = Position.ZERO_XYZ; // position only, the "t" field is not used
//	protected Velocity v = Velocity.ZERO; // velocity
//	public Plan trajectory = new Plan();               // Current trajectory of aircraft derived from flight plan
//	public NavPoint chasePoint = NavPoint.ZERO_XYZ;
//	protected ErrorLog error = new ErrorLog("Aircraft");
//
//	public Aircraft(String id) {
//	  id_ = id;
//	}
//	
//	public String getCallSign() {
//	  return id_;
//	}
//	
//	public void addError(String s) {
//		error.addError(s);
//	}
//	
//	// ErrorReporter Interface Methods
//
//	public boolean hasError() {
//		return error.hasError() || trajectory.hasError();
//	}
//	public boolean hasMessage() {
//		return error.hasMessage() || trajectory.hasMessage();
//	}
//	public String getMessage() {
//		return id_+":\n"+error.getMessage() + trajectory.getMessage();
//	}
//	public String getMessageNoClear() {
//		return id_+":\n"+error.getMessageNoClear() + trajectory.getMessageNoClear();
//	}
//}
