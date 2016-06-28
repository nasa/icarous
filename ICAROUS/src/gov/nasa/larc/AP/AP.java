/*
 * Copyright (c) 2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
package gov.nasa.larc.AP;

import gov.nasa.larc.AP.APPacket.Context;
import gov.nasa.larc.ICAROUS.Icarous;
import gov.nasa.larc.serial.TextWriter;
import gov.nasa.larcfm.Util.Plan;
import gov.nasa.larcfm.Util.NavPoint;

public class AP
{


  //====================================================================================================================//
  // DATA Section
  //====================================================================================================================//

  public static final int TAKEOFF_RPM = 4800;
  public static final int CRUISE_RPM = 3800;
  public static final int IDLE = 200;
  // enum version of MavLink mode info from AP
  // public short	 mode;	 	// mode: MANUAL=0, CIRCLE=1, STABILIZE=2, FLY_BY_WIRE_A=5, FLY_BY_WIRE_B=6, FLY_BY_WIRE_C=7, AUTO=10, RTL=11, LOITER=12, GUIDED=15, INITIALISING=16
  //public enum APModeEnum { AUTO, MANUAL, RTL };
  public static enum APModeEnum { MANUAL, CIRCLE, STABILIZE, FLY_BY_WIRE_A, FLY_BY_WIRE_B, FLY_BY_WIRE_C, AUTO, RTL, LOITER, GUIDED, INITIALISING};

  public static enum movementStatus { STOPPED, TAXI, TAKEOFF, FLYING, LANDING };
  public movementStatus moveStat = movementStatus.STOPPED;

  public static APPacket Packet;	// This is the AP message decoder and encoder

  public static MavLink_0_9 data = new MavLink_0_9();	// Stores all AP parameters as broadcasted from AP and when requested in conversations

  //=====================================================================================//
  // AP Flight Plan
  // ------------------------------>>>
  //
  public APPlan launchPlan;
  public APPlan currentPlan;
  // <<<------------------------------
  // end of AP Flight Plan variables


  public APListener serial;		// = new SerialPort(portName);

  public APLinkMonitor APLink;	// = new APLinkMonitor("APLinkMonitorLog.csv");
  public boolean logLink = false;

  String fplogFileName = null;
  TextWriter fplogFile = null;
  public boolean logPlans = false;
  private int lastLookWPNum;
  private int loiterWPIdx = -1;		// should be invalid until first loiter point reached.  Then updated everytime we get to loiter.

  //====================================================================================================================//
  // FUNCTION Section
  //====================================================================================================================//
  //=====================================================================================//
  /**
   * Constructor to open serial port.
   * @param pn   Serial port logical name.
   */
  //=====================================================================================//
  public AP(String pn) 
  {

    APLink = new APLinkMonitor(Icarous.logFileNamePrefix+"APLinkMonitorLog.csv");
    logLink = true;

    Packet = new APPacket();
    APPacket.APLink = APLink;
    serial = new APListener(pn);
    APPacket.serial = serial;

    fplogFileName = Icarous.logFileNamePrefix+"APPlanHistory.txt";
    fplogFile = new TextWriter(fplogFileName);
    logPlans = true;

  }



  //=====================================================================================//
  /**
   * Get the original plan from AP.  
   * 
   */
  //=====================================================================================//
  public void loadLaunchPlan ()
  {
    if (launchPlan == null)
    {
      this.launchPlan = new APPlan("LaunchPlan");
      Packet.getAPFlightPlan(this.launchPlan);			
    }
    //return fp.launchAPPlan;
  }


  //=====================================================================================//
  /**
   * Get the original plan from AP.  
   * 
   */
  //=====================================================================================//
  public boolean hasLaunchPlan()
  {
    if (  (launchPlan != null)
        && (launchPlan.isReadyForUse() )  )
    {
      return true;			
    } 
    return false;
  }


  //=====================================================================================//
  /**
   * Returns the original AP plan object in this APFlightPlan.
   * @return
   */
  //=====================================================================================//
  //	public Plan getLaunchPlan ()
  //	{
  //		if ( (fp.CDnRplan == null) || (fp.CDnRplan.size() != fp.launchAPPlan.size()-1) )
  //		{
  //			fp.getPlan(0);			
  //		}
  //		return fp.CDnRplan;
  //	}

  //=====================================================================================//
  /**
   * Reload the launch plan into the AP hardware.  Use this function to reset scenarios
   * when safety pilot scrubs the test.  Simulation clock in GlobalState is assumed to be
   * reset to zero on the airplane and the ground simulator.
   * 
   * @return true if successful.
   */
  //=====================================================================================//
  public boolean reloadAPlaunchPlan ()
  {	

    //		Packet.sendAPFlightPlan(fp.launchAPPlan);
    Packet.sendAPFlightPlan(this.launchPlan);
    return true;
  }	// end function

  //=====================================================================================//
  /**
   * Convert the given Plan to an APPlan and send to AP hardware.  If not successful sending,
   * then bank the plan in packet and let packet send on next available opportunity.
   * @param pn   Serial port logical name.
   */
  //=====================================================================================//
  //	public boolean sendNewCDnRPlan (Plan newPlan, double simTime)
  //	{	
  //		boolean status = false;
  //		System.err.println("-----------------------------------------------\n"
  //				+ Thread.currentThread().getName()
  //				+ "--> AP::sendNewCDnRPlan: Preparing to send new Stratway plan:"
  //				+ newPlan.toString()
  //				+ "-----------------------------------------------\n"
  //				);
  //		ArrayList<APWayPoint> newAPPlan = fp.PlanToAPPlan(newPlan, simTime);
  //		System.out.println(Thread.currentThread().getName()
  //							+ "--> AP::sendNewCDnRPlan: new AP plan:"
  //							);
  //		fp.printPlan(newAPPlan);
  //
  //		if ( Packet.sendAPFlightPlan(newAPPlan) )
  //		{			
  //			return true;
  //		} else {		// save the flight plan for 
  //			System.err.println("-----------------------------------------------\n"
  //							+Thread.currentThread().getName()
  //							+ "--> AP::sendNewCDnRPlan: Ignored flightplan change because still trying to transmit previous trajectory change.:"
  //							+"-----------------------------------------------\n"
  //							);
  //			//Packet.sendWhenReady(newAPPlan);	// bank plan in Packet and handle sending newAPPlan in when serial port is ready for new conversation
  //	
  //			return false;
  //		}
  //	}

  //=====================================================================================//
  /**
   * Convert the given Plan to an APPlan and send to AP hardware.  If not successful sending,
   * then bank the plan in packet and let packet send on next available opportunity.
   * @param PlanCore   the plan to send
   * @param double   	 the base time for offset
   * @param int   dummy place holder to differentiate from other sendNewCDnRPlan() function
   */
  //=====================================================================================//
  public boolean sendNewCDnRPlan (Plan newPlan, double simTime)
  {	
    APPlan newAPPlan = new APPlan(newPlan, simTime);
    System.err.println(
        "-----------------------------------------------\n"
            + Thread.currentThread().getName()
            + "--> AP::sendNewCDnRPlan:  Stratway plan:"
            + newPlan.toString()
            + "-----------------------------------------------\n"
            + "--> AP::sendNewCDnRPlan: new AP plan:"
            + newAPPlan.toString()
            + "-----------------------------------------------\n"
        );
    if (logPlans ) {
      this.logTime(simTime);
      logFlightPlan (newAPPlan);				// log flight plan
    }
    if ( Packet.sendAPFlightPlan(newAPPlan) )	// send plan to AP
    {			
      return true;
    } else {		// save the flight plan for 
      System.err.println("-----------------------------------------------\n"
          +Thread.currentThread().getName()
          + "--> AP::sendNewCDnRPlan: Ignored this flightplan change because AP Hardware still getting previous trajectory change."
          +"-----------------------------------------------\n"
          );
      //Packet.sendWhenReady(newAPPlan);	// bank plan in Packet and handle sending newAPPlan in when serial port is ready for new conversation

      return false;
    }
  }

  //=====================================================================================//
  /**
   * Replaces the data in waypoint "num" with the lat/lon/alt from "nowPt" and the time is
   * the delta between "prevPt" and "nowPt".
   * @param pn   Serial port logical name.
   */
  //=====================================================================================//
  public void changeWP (short num, NavPoint nowPt, NavPoint prevPt) 
  {
    APWayPoint newWP = new APWayPoint(num, nowPt, prevPt); 
    newWP.current = 255;
    APPacket.serial.send(MavLink_0_9.WAYPOINT.encode());
  }

  //=====================================================================================//
  /**
   * Vector the airplane to a heading for a given time.  When time expires, fly back to 
   * current waypoint.  
   * 
   * @param v_h_flag   	0=no hdg change, 1=fly to given hdg.
   * @param v_s_flag   	0=no speed change, 1=fly at given speed.
   * @param v_a_flag   	0=no alt change info, 1=fly to given alt.
   * @param v_t_flag   	0=stay on given parameters indefinitely, 1=fly for given time.
   * @param v_new_heading   	New hesding if desired.  This value will be used only if v_h_flag = 1.
   * @param v_new_airspeed   	New airspeed if desired.  This value will be used only if v_s_flag = 1.
   * @param v_new_altitude   	New altitude if desired.  This value will be used only if v_a_flag = 1.
   * @param v_max_time   		New time if desired.  This value will be used only if v_t_flag = 1.
   */
  //=====================================================================================//
  public void vectorTo (byte v_h_flag
      , byte v_s_flag
      , byte v_a_flag
      , byte v_t_flag
      , short v_new_heading
      , short v_new_airspeed
      , short v_new_altitude
      , short v_max_time
      )
  {

    APPacket.serial.send(MavLink_0_9.CDNR_CONTROLLER.encode(   (byte) v_h_flag
        , (byte) v_s_flag
        , (byte) v_a_flag
        , (byte) v_t_flag
        , (short) v_new_heading
        , (short) v_new_airspeed
        , (short) v_new_altitude
        , (short) v_max_time
        )
        );
    if (this.logPlans) {
      fplogFile.write(	"Vector Command issued ["+System.currentTimeMillis()+"] "
          +"comnmand ["
          +"Hdg ["+v_h_flag+":"+v_new_heading+"]"
          +"GndSpeed ["+v_s_flag+":"+v_new_airspeed+"]"
          +"Alt ["+v_a_flag+":"+v_new_altitude+"]"
          +"Time ["+v_t_flag+":"+v_max_time+"]"
          +"] "
          );
    }

  }

  //=====================================================================================//
  /**
   * Return the string representing the number
   * Vehicle number, e.g. "R1" if 1, "R2" if 2, "R3" if 3
   * @returns true if autopilot is currently loitering.  false otherwise.
   */
  //=====================================================================================//
  public String getName() {
    String name = null;

    switch(MavLink_0_9.STATE_DATA.num)
    {
    case 1: name = "R1"; break;
    case 2: name = "R2"; break;
    case 3: name = "R3"; break;
    default: name = "R2";
    }
    return name;
  }

  //=====================================================================================//
  /**
   * @returns true if autopilot is currently loitering.  false otherwise.
   */
  //=====================================================================================//
  public boolean isAtLoiter() {
    boolean result = MavLink_0_9.STATE_DATA.atloiter==1?true:false;
    if (result == true)
    {
      loiterWPIdx  = getCurrentWPNum();
    }
    return (result);
  }



  //=====================================================================================//
  /**
   * @returns the current RPM
   */
  //=====================================================================================//
  public float getRPM() {
    //return data.ANALOG_EU.chan12;

    float RPM = MavLink_0_9.ANALOG_EU.chan12;
    //System.err.println("AP-->getRPM(): APport " + WatchConfig.APport + " RPM " + RPM);
    // [CAM] THE FOLLOWING CODE DOESN'T SEEM TO APPLY TO ICAROUS
    /*
		String checkAPport = "COM22";	
		if(WatchConfig.APport.equals(checkAPport))
			{
			RPM = AP.TAKEOFF_RPM+1000;		// for runnign in the lab against HILSim Edge only.
			}
     */
    if (RPM > AP.TAKEOFF_RPM)
      this.moveStat = movementStatus.FLYING;
    return RPM;
  }

  //=====================================================================================//
  /**
   * 	public short	 mode;	 	// mode: MANUAL=0, CIRCLE=1, STABILIZE=2, FLY_BY_WIRE_A=5, FLY_BY_WIRE_B=6, FLY_BY_WIRE_C=7, AUTO=10, RTL=11, LOITER=12, GUIDED=15, INITIALISING=16
   * @returns the AP mode which is anticipated to be the Aux2 switch position on pilot's radio.
   */
  //=====================================================================================//
  public short getMode() {
    return MavLink_0_9.STATE_DATA.mode;
  }

  //=====================================================================================//
  /**
   * 
   * 	MavLink defined mode: MANUAL=0, CIRCLE=1, STABILIZE=2, FLY_BY_WIRE_A=5, FLY_BY_WIRE_B=6, FLY_BY_WIRE_C=7, AUTO=10, RTL=11, LOITER=12, GUIDED=15, INITIALISING=16
   * @returns the AP mode enumeration 
   */
  //=====================================================================================//
  public static APModeEnum getModeEnum(short m) {

    APModeEnum apmode = APModeEnum.MANUAL;

    switch (m)
    {
    case 0: apmode = APModeEnum.MANUAL; break;
    case 1: apmode = APModeEnum.CIRCLE; break;
    case 2: apmode = APModeEnum.STABILIZE; break;
    case 5: apmode = APModeEnum.FLY_BY_WIRE_A; break;
    case 6: apmode = APModeEnum.FLY_BY_WIRE_B; break;
    case 7: apmode = APModeEnum.FLY_BY_WIRE_C; break;
    case 10: apmode = APModeEnum.AUTO; break;
    case 11: apmode = APModeEnum.RTL; break;
    case 12: apmode = APModeEnum.LOITER; break;
    case 15: apmode = APModeEnum.GUIDED; break;
    case 16: apmode = APModeEnum.INITIALISING; break;
    default: apmode = APModeEnum.MANUAL;
    }
    return apmode;
  }

  public APModeEnum getModeEnum() 
  {
    return getModeEnum( getMode() );
    //return AP.APModeEnum.AUTO;
  }

  //=====================================================================================//
  /**
   * 
   * @returns the string for the AP mode enumeration 
   */
  //=====================================================================================//
  public static String getModeText( short m) 
  {
    return getModeEnum(m).toString();
  }	


  public String getModeText() 
  {
    return getModeEnum().toString();
  }	

  //=====================================================================================//
  /**
   * 
   * @returns the string for the AP movement status.  Is airplane stopped, flying, taxi,...
   */
  //=====================================================================================//
  public String getMovementStatusText () 
  {
    return moveStat.toString();
  }	

  //=====================================================================================//
  /**
   * Returns the current target waypoint. The one the AP is flying to.  Returns -1 if data
   * is not yet available or invalid.
   * 
   * @param pn   Serial port logical name.
   */
  //=====================================================================================//
  public boolean currentWPNumChanged () 
  {
    boolean result = false;
    if (moveStat == movementStatus.FLYING)
    {
      if (lastLookWPNum != getCurrentWPNum())
      {
        lastLookWPNum = getCurrentWPNum();
        result = true;
      } else 
        result = false;
    }
    return result; 

  }

  //=====================================================================================//
  /**
   * Returns the current target waypoint. The one the AP is flying to.  Returns -1 if data
   * is not yet available or invalid.
   * 
   * @param pn   Serial port logical name.
   */
  //=====================================================================================//
  public int getCurrentWPNum () 
  {
    return MavLink_0_9.WAYPOINT_CURRENT.seq; 

  }

  //=====================================================================================//
  /**
   * Tell AP to set current waypoint to the given number.  AP will immediately set course
   * to given waypoint and fly there.
   * @param pn   Serial port logical name.
   */
  //=====================================================================================//
  public boolean setCurrentWPNum (int newSeq) 
  {
    return serial.send(MavLink_0_9.WAYPOINT_SET_CURRENT.encode(newSeq)); 
  }

  //=====================================================================================//
  /**
   * 
   * @param pn   Serial port logical name.
   */
  //=====================================================================================//
  public void getAPFlightPlan() 
  {
    if (APPacket.conversationContext == Context.stateless)
    {
      Packet.getAPFlightPlan();			
    }


  }

  //=====================================================================================//
  /**
   * 
   * @param pn   Serial port logical name.
   */
  //=====================================================================================//
  public boolean APplanReceived ()
  {
    return APPacket.APplanReceived;
  }



  //=====================================================================================//
  /**
   * Converts APMode enumeration to APMode byte.  Need to match the byte number to MavLink
   * encoding for APMode.
   * 
   * @return byte.
   */
  //=====================================================================================//
  public static byte toAPModeByte (APModeEnum a) {
    byte b = 0x01;
    switch (a)	
    {
    case AUTO:   
      b =  0x00;
      break;
    case MANUAL: 
      b =  0x01;
      break;
    case RTL: 	 
      b =  0x02;
      break;
    default:
      break;
    }
    return b;
  }


  //=====================================================================================//
  /**
   * Converts APMode byte to APMode enumeration.  Need to match the byte number to MavLink
   * encoding for APMode.
   * 
   * @return APMode enumeration.
   */
  //=====================================================================================//
  public static APModeEnum toAPModeEnum (byte a) {
    APModeEnum en = APModeEnum.MANUAL;
    switch (a)	
    {
    case 0x00:   en = APModeEnum.AUTO;
    case 0x01: 	 en = APModeEnum.MANUAL;
    case 0x02: 	 en = APModeEnum.RTL;
    }
    return en;
  }

  //=====================================================================================//
  /**
   * Replaces the data in waypoint "num" with the lat/lon/alt from "nowPt" and the time is
   * the delta between "prevPt" and "nowPt".
   * @param pn   Serial port logical name.
   */
  //=====================================================================================//
  public void logFlightPlan (APPlan p)
  {
    if ( (fplogFile != null ) && (p != null) )
    {
      fplogFile.write(p.toString());
    }

  }

  public void logFlightPlan ()
  {
    if ( (fplogFile != null ) && (launchPlan != null) )
    {
      fplogFile.write(launchPlan.toString());
    }

  }

  //=====================================================================================//
  /**
   * Write string to AP's flight plan log
   * @param pn   Serial port logical name.
   */
  //=====================================================================================//
  public boolean logAString (String theStringToLog)
  {
    if ( (fplogFile != null ) && (logPlans) )
    {
      //fplogFile.write(theStringToLog);
      return true;
    }
    return false;
  }

  public boolean logTime (double simTime)
  {
    return logAString ( "SystemTime: ["+ System.currentTimeMillis()+"] ms, SimTime [" + simTime+"]");

  }

  //====================================================================================================================//
  // MAIN
  //====================================================================================================================//

  //=====================================================================================//
  /**
   * Entry point to test AP communication and data storage.
   * @param pn   Serial port logical name.
   */
  //=====================================================================================//
  public static void main(String[] args) 
  {

    //String APPortName = "COM23";	// direct connect
    String APPortName = "COM22";	// R2 USB to Serial Device
    //String APPortName = "COM24";	// R2 USB to Serial Device
    //String APPortName = "/dev/ttyUSB2";	// R2 USB to Serial Device
    System.out.println( Thread.currentThread().getName()
        + " --> AP::Main: AP running standalone listening to port ["+APPortName+"]"
        );

    // create serial port listeners and parsers
    //		System.out.println( Thread.currentThread().getName()
    //							+ " --> AP::Main: Initializing Serial Driver..."
    //						);		

    AP ap = new AP(APPortName);

    ap.getAPFlightPlan();
    while (!ap.APplanReceived()) {
      try {
        Thread.sleep(5);
      } catch (InterruptedException e) {
        // TODO Auto-generated catch block
        e.printStackTrace();
      }
    }

    //		TextLogger logger = new TextLogger((float)1.0, "APLog.csv", WAYPOINT);		
    //		logger.start();

    //		ap.sendAPFlightPlan();
    //		edge.AP.fp.updatePlan((double)0.0);
    //		apListener.sendAPFlightPlan( edge.AP.fp.getCurrentAPPlan() );
    while (true)
    {
      try {
        Thread.sleep(500);
      } catch (InterruptedException e) {
        // TODO Auto-generated catch block
        e.printStackTrace();
      }
    }


  }	// end main()



  public boolean atLoiter() {
    // TODO Auto-generated method stub
    return false;
  }



  public boolean geoFenceViolated() {
    // TODO Auto-generated method stub
    return MavLink_0_9.FENCE_STATUS.breach_status != 0;
  }

  public boolean syncTrafTimingInfo(float timeToWP){
    // [CAM] Originally, the code used GlobalState.getSimTime(). Is this needed here?
    serial.send(MavLink_0_9.TRAFFIC_SIM_TIMING.encode((float)Icarous.getSimTime(), timeToWP	 ));
    return true;
  }
  public boolean goToNextWayPoint() {

    serial.send( MavLink_0_9.WAYPOINT_SET_CURRENT.encode(MavLink_0_9.WAYPOINT_CURRENT.seq+1) );
    return true;
  }

  public boolean goToScenarioStartWayPoint() {

    //serial.send( MavLink_0_9.WAYPOINT_SET_CURRENT.encode(2) );
    serial.send( MavLink_0_9.WAYPOINT_SET_CURRENT.encode(loiterWPIdx + 1) );
    return true;
  }


  public void setCurrentPlan() {
    this.currentPlan = new APPlan(this.launchPlan);
  }

  //	public boolean setWayPoint(int wp) {
  //		
  //		serial.send( MavLink_0_9.WAYPOINT_SET_CURRENT.encode(wp) );
  //		return true;
  //	}







}		// end AP class
