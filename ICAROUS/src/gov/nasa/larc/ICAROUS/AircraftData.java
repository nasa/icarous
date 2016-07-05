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
import java.util.*;

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
    public Position aircraftPosition;

    // Angle of attack, sideslip and airspeed
    public double aoa;
    public double sideslip;
    public double airspeed;

    boolean sendmsg;
    
    public FlightPlan CurrentFlightPlan;
    
    List Obstacles = new ArrayList(); // List for obstacles
    List Traffic   = new ArrayList(); // List for traffic information

    public int startMission = -1; // -1: last command executed, 0 - stop mission, 1 - start mission
    
    public AircraftData(boolean msg_requirement){
	if(msg_requirement){
	    RcvdMessages = new MAVLinkMessages();
	}

	aircraftPosition    = new Position();
	CurrentFlightPlan   = new FlightPlan();
    }

    public void CopyAircraftStateInfo(AircraftData Input){

	roll  = Input.roll;
	pitch = Input.pitch;
	yaw   = Input.yaw;

	roll_rate  = Input.roll_rate;
	pitch_rate = Input.pitch_rate;
	yaw_rate   = Input.yaw_rate;

	p = Input.p;
	q = Input.q;
	r = Input.r;

	u = Input.u;
	v = Input.v;
	w = Input.w;

	aircraftPosition.UpdatePosition(Input.aircraftPosition.lat,
					Input.aircraftPosition.lon,
					Input.aircraftPosition.alt_msl,
					Input.aircraftPosition.alt_agl);

	aoa      = Input.aoa;
	sideslip = Input.sideslip;
	airspeed = Input.airspeed;
    }

    public void GetDataFromMessages(){

	double lat = (double) (RcvdMessages.msgGlobalPositionInt.lat)/1.0E7;
	double lon = (double) (RcvdMessages.msgGlobalPositionInt.lon)/1.0E7;
	double alt_msl = (double) (RcvdMessages.msgGlobalPositionInt.alt)/1.0E3;
	double alt_agl = (double) (RcvdMessages.msgGlobalPositionInt.relative_alt)/1.0E3;

	aircraftPosition.UpdatePosition((float)lat,(float)lon,(float)alt_msl,(float)alt_agl);
    }

}
