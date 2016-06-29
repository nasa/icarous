/**
 * ICAROUS Interface
 * Contact: Swee Balachandran (swee.balachandran@nianet.org)
 * 
 * 
 * Copyright (c) 2011-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
package gov.nasa.larc.ICAROUS;

public class AircraftData{

    public static boolean INIT_MESSAGES = true;
    public static boolean NO_MESSAGES   = false;
    
    public MAVLinkMessages RcvdMessages;

    // Aircraft attitude
    public double roll;
    public double pitch;
    public double yaw;

    // Aircraft euler rates
    public double roll_rate;
    public double pitch_rate;
    public double yaw_rate;

    // Aircraft angular rates
    public double p;
    public double q;
    public double r;

    // Aircraft velocity components in the body frame
    public double u;
    public double v;
    public double w;

    // Aircraft position (GPS)
    public double lat;
    public double lon;
    public double alt;

    // Angle of attack, sideslip and airspeed
    public double aoa;
    public double sideslip;
    public double airspeed;

    public AircraftData(boolean msg_requirement){
	if(msg_requirement){
	    RcvdMessages = new MAVLinkMessages();
	}
    }

    public void CopyAircraftStateInfo(AircraftData Input){

	roll = Input.roll;
	pitch = Input.pitch;
	yaw = Input.pitch;

	roll_rate = Input.roll_rate;
	pitch_rate = Input.pitch_rate;
	yaw_rate = Input.yaw_rate;

	p = Input.p;
	q = Input.q;
	r = Input.r;

	u = Input.u;
	v = Input.v;
	w = Input.w;

	lat = Input.lat;
	lon = Input.lon;
	alt = Input.alt;

	aoa = Input.aoa;
	sideslip = Input.sideslip;
	airspeed = Input.airspeed;
    }

    public void GetDataFromMessages(){


	lat = (double) (RcvdMessages.msgGlobalPositionInt.lat)/1.0E7;
	lon = (double) (RcvdMessages.msgGlobalPositionInt.lon)/1.0E7;
	alt = (double) (RcvdMessages.msgGlobalPositionInt.alt)/1.0E3;
    }

}
