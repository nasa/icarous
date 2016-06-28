/*
 * Copyright (c) 2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
/**
 * 
 */
package gov.nasa.larc.AP;

import gov.nasa.larc.AP.WAYPOINT_class;
import gov.nasa.larcfm.Util.NavPoint;
import gov.nasa.larcfm.Util.Position;
import gov.nasa.larcfm.Util.Units;

/**
 *  Convenience class for storing AP Waypoint data.
		{'target_system'}    ... %% System ID
		{'target_component'} ... %% Component ID
		{'seq'}              ... %% Sequence
		{'frame'}            ... %% The coordinate system of the waypoint. see MAV_FRAME in mavlink_types.h
		{'command'}          ... %% The scheduled action for the waypoint. see MAV_COMMAND in common.xml MAVLink specs
		{'current'}          ... %% false:0, true:1 set =255 if want to send a waypoint.
		{'autocontinue'}     ... %% autocontinue to next wp
		{'param1'}           ... %% PARAM1 / For NAV command waypoints: Radius in which the waypoint is accepted as reached, in meters
		{'param2'}           ... %% PARAM2 / For NAV command waypoints: Time that the MAV should stay inside the PARAM1 radius before advancing, in milliseconds
		{'param3'}           ... %% PARAM3 / For LOITER command waypoints: Orbit to circle around the waypoint, in meters. If positive the orbit direction should be clockwise, if negative the orbit direction should be counter-clockwise.
		{'param4'}           ... %% PARAM4 / For NAV and LOITER command waypoints: Yaw orientation in degrees, [0..360] 0 = NORTH
		{'x'}                ... %% PARAM5 / local: x position, global: latitude
		{'y'}                ... %% PARAM6 / y position: global: longitude
		{'z'}                ... %% PARAM7 / z position: global: altitude

 * @author cquach
 *
 */
public class APWayPoint	extends WAYPOINT_class
{
	public static final short WPTYPE_LATLON = (short)16;
	public static final short WPTYPE_LOITER = (short)17;
	public static final short WPTYPE_JUMP = (short)177;
	public static short DEFAULT_TARGET_SYS = (short)1;
	public static short DEFAULT_TARGET_COMP = (short)1;
	public static short DEFAULT_FRAME = (short)0;
	
	//=====================================================================================//
	/**
	 * Parse a byte packet and extract waypoint info into the object
	 */
	//=====================================================================================//
	public APWayPoint(byte[] packet) {
		//parse(packet);
	}	// end APWayPoint() constructor
	
	//=====================================================================================//
	/**
	 * Copy constructor.
	 */
	//=====================================================================================//
	public APWayPoint (APWayPoint o) {
		target_system = o.target_system;	 	// System ID
		target_component = o.target_component; 	// Component ID
		seq = o.seq;				 	// Sequence
		frame = o.frame;			 	// The coordinate system of the waypoint. see MAV_FRAME in mavlink_types.h
		command = o.command;			 	// The scheduled action for the waypoint. see MAV_COMMAND in common.xml MAVLink specs
		current = o.current;			 	// false:0, true:1
		autocontinue = o.autocontinue;	 	// autocontinue to next wp
		param1 = o.param1;			 	// PARAM1 / For NAV command waypoints: Radius in which the waypoint is accepted as reached, in meters
		param2 = o.param2;			 	// PARAM2 / For NAV command waypoints: Time that the MAV should stay inside the PARAM1 radius before advancing, in milliseconds
		param3 = o.param3;			 	// PARAM3 / For LOITER command waypoints: Orbit to circle around the waypoint, in meters. If positive the orbit direction should be clockwise, if negative the orbit direction should be counter-clockwise.
		param4 = o.param4;			 	// PARAM4 / For NAV and LOITER command waypoints: Yaw orientation in degrees, [0..360] 0 = NORTH
		x = o.x;				 	// PARAM5 / local: x position, global: latitude
		y = o.y;				 	// PARAM6 / y position: global: longitude
		z = o.z;				 	// PARAM7 / z position: global: altitude
	}	// end APWayPoint() constructor
	
	//=====================================================================================//
	/**
	 * Copy constructor from a WAYPOINT_class object.
	 */
	//=====================================================================================//
	public APWayPoint (WAYPOINT_class o) 
	{
		target_system = o.target_system;	 	// System ID
		target_component = o.target_component; 	// Component ID
		seq = o.seq;				 	// Sequence
		frame = o.frame;			 	// The coordinate system of the waypoint. see MAV_FRAME in mavlink_types.h
		command = o.command;			 	// The scheduled action for the waypoint. see MAV_COMMAND in common.xml MAVLink specs
		current = o.current;			 	// false:0, true:1
		autocontinue = o.autocontinue;	 	// autocontinue to next wp
		param1 = o.param1;			 	// PARAM1 / For NAV command waypoints: Radius in which the waypoint is accepted as reached, in meters
		param2 = o.param2;			 	// PARAM2 / For NAV command waypoints: Time that the MAV should stay inside the PARAM1 radius before advancing, in milliseconds
		param3 = o.param3;			 	// PARAM3 / For LOITER command waypoints: Orbit to circle around the waypoint, in meters. If positive the orbit direction should be clockwise, if negative the orbit direction should be counter-clockwise.
		param4 = o.param4;			 	// PARAM4 / For NAV and LOITER command waypoints: Yaw orientation in degrees, [0..360] 0 = NORTH
		x = o.x;				 	// PARAM5 / local: x position, global: latitude
		y = o.y;				 	// PARAM6 / y position: global: longitude
		z = o.z;				 	// PARAM7 / z position: global: altitude		
	}
	//=====================================================================================//
	/**
	 * Create a waypoint given a specific time to use as the lapse time and get lat/lon/alt
	 * from a given NavPoint.
	 * @param isCurrentWP 
	 * @param point
	 */
	//=====================================================================================//
	public APWayPoint( short wpSeq, NavPoint p0, float lapseTime, short isCurrentWP) 
	{	
		
		init (	(short) wpSeq	 	 	// Sequence
				,(short) isCurrentWP			 	// is current waypoint? false:0, true:1
				,command = WPTYPE_LATLON
				,(float) Units.to("deg",p0.lat())
				,(float) Units.to("deg",p0.lon())
				,(float) Units.to(Units.meter,p0.alt())
				,(float) lapseTime
				,(float) 0.0			// Param2
				,(float) 0.0			// Param3
				,(float) 0.0			// Param4
				);
		
	}


	//=====================================================================================//
	/**
	 * Convenience function create a new waypoint in the AP format..
	 * @return a new AP waypoint  
	 */
	//=====================================================================================//
	public APWayPoint ( short wpSeq, NavPoint p0, NavPoint prev)
	{
		init (	(short) wpSeq	 	 		// Sequence
				,(short) 0			 		// is current waypoint? false:0, true:1
				,command = WPTYPE_LATLON
				,(float) Units.to("deg",p0.lat())
				,(float) Units.to("deg",p0.lon())
				,(float) Units.to(Units.meter,p0.alt())
				,(float) (p0.time() - prev.time())
				,(float) 0.0		// Param2
				,(float) 0.0		// Param3
				,(float) 0.0		// Param4
				);
	}
	
	//=====================================================================================//
	/**
	 * Create a waypoint which is a jump command to loop back to another point
	 * @param point
	 */
	//=====================================================================================//
	public APWayPoint( short wpSeq, float jumpToPoint, float interations) 
	{	
		
		init (	(short) wpSeq	 	 	// Sequence
				,(short) 0			 	// is current waypoint? false:0, true:1
				,command = WPTYPE_JUMP
				,(float) 0.0			// lat
				,(float) 0.0			// lon
				,(float) 0.0			// alt
				,(float) jumpToPoint	// param1
				,(float) interations	// Param2
				,(float) 0.0			// Param3
				,(float) 0.0			// Param4
				);
		
	}

	
	//=====================================================================================//
	/**
	 * Do nothing constructor.  Will be phased out.
	 * @param point
	 */
	//=====================================================================================//
	public APWayPoint() {
		// TODO Auto-generated constructor stub
	}

	//=====================================================================================//
	/**
	 * Initialize waypoint object given partial internal data.  Sets any unspecified 
	 * parameters to defaults.  Use this for creating latlon waypoints.
	 * @param point
	 */
	//=====================================================================================//
	public void init (short	 s				 	// Sequence
					,short	 cur			 	// false:0, true:1
					,short com
					,float xi
					,float yi
					,float zi
					,float p1
					,float p2
					,float p3
					,float p4
					)
	{	
		
		target_system = DEFAULT_TARGET_SYS;
		target_component = DEFAULT_TARGET_COMP;
		seq = (short) s;
		frame = DEFAULT_FRAME;
		command = com;
		current = 0;
		param1 = p1;
		param2 = p2;
		param3 = p3;
		param4 = p4;
		x = xi;
		y = yi;
		z = zi;
		
	}	// end init() partial spec.

	//=====================================================================================//
	/**
	 * Initialize waypoint object given full internal data.
	 * @param point
	 */
	//=====================================================================================//
	public void init (short	 ts		// System ID
					,short	 tc 	// Component ID
					,int	 s				 	// Sequence
					,short	 fr			 	// The coordinate system of the waypoint. see MAV_FRAME in mavlink_types.h
					,short	 cmd			 	// The scheduled action for the waypoint. see MAV_COMMAND in common.xml MAVLink specs
					,short	 cur			 	// false:0, true:1
					,short	 jumpTo	 	// autocontinue to next wp
					,float p1
					,float p2
					,float p3
					,float p4
					,float xi
					,float yi
					,float zi
					)
	{	
		
		target_system = ts;
		target_component = tc;
		seq = (short) s;
		frame = fr;
		command = cmd;
		current = cur;
		autocontinue = jumpTo;
		param1 = p1;
		param2 = p2;
		param3 = p3;
		param4 = p4;
		x = xi;
		y = yi;
		z = zi;
		
	}	// end init() full spec.


	
	//=====================================================================================//
	/**
	 * Rename data per MavLink comments.  Note "param1" has several meaning depending on 
	 * waypoint command.
	 */
	//=====================================================================================//
	public float getTime()
	{
		return param1;

	}	// end getTime() function


	//=====================================================================================//
	/**
	 * Returns the WP frame.
	 */
	//=====================================================================================//
	public int getFrame()
	{
		return (int)frame;

	}	// end getType() function


	//=====================================================================================//
	/**
	 * Accessor function for command
	 */
	//=====================================================================================//
	public int getCommand()
	{
		return (int)command;

	}	// end getTime() function

	//=====================================================================================//
	/**
	 * Accessor function for command.  Called type because the command number tell which 
	 * type of waypoint. eg latlon or jump.
	 */
	//=====================================================================================//
	public int getType()
	{
		return (int)command;

	}	// end getTime() function

	
	//=====================================================================================//
	/**
	 * Accesspr for Sequence number
	 */
	//=====================================================================================//
	public int getSeq()
	{
		return seq;

	}	// end getTime() function


	//=====================================================================================//
	/**
	 * Rename data per MavLink comments
	 */
	//=====================================================================================//
	public float radius()
	{
		return param1;

	}	// end radius() function


	//=====================================================================================//
	/**
	 * Rename data per MavLink comments
	 */
	//=====================================================================================//
	public float orbits()
	{
		return param2;

	}	// end toPacket() function

	//=====================================================================================//
	/**
	 * Set this to be the current waypoint
	 */
	//=====================================================================================//
	public void setCurrent()
	{
		current = 1;

	}	// end toPacket() function

	//=====================================================================================//
	/**
	 * Create a Stratway NavPoint from internally AP waypoint info.  Transfer the field
	 * which apply.  No error checking on whether this is a valid 
	 */
	//=====================================================================================//
	public NavPoint toNavPoint(double prevTime) 
	{
		NavPoint np = new NavPoint( Position.makeLatLonAlt(x, y, Units.to("ft",z) ), prevTime);
		// may need to set point mutability.
		return np;
	}

	//=====================================================================================//
	/**
	 * Stringafy the data in this object.
	 * 
	 */
	//=====================================================================================//
	public String toString ()
	{
		String wpStr = "WP["+this.seq+"] SystemID[" + this.target_system +"] "+ "current flag[" + this.current +"]";
		if (seq == 0) {
			wpStr +=  " Type ["+this.command+": Home] [" + this.x + "," + this.y + "," + this.z + "]";
			return wpStr;
		}
		switch (this.command)
		{
		case WPTYPE_LOITER :
		case WPTYPE_LATLON :
			wpStr += " Type ["+this.command+": LatLon] [" + this.x + "," + this.y + "," + this.z + "," + this.param1+"]";
			break;
		case WPTYPE_JUMP :
			wpStr += " Type ["+this.command+": Jump] [ Jump to WP Index:" + this.param1 +"]";
			break;
		default:
			break;
		}
		
		return wpStr;
	
	}	// end toString() function
	
	
	
}	// END APWayPoint CLASS
