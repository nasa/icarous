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
import gov.nasa.larc.ICAROUS.*;

public class SITL_ComTest{

    public static void main(String args[]){
	AircraftData SharedData    = new AircraftData(AircraftData.INIT_MESSAGES);
	ICAROUS_Interface COMint   = new ICAROUS_Interface(ICAROUS_Interface.COMBOX,  /*Interface type */
							   args[0],                   /*Host name */
							   Integer.parseInt(args[1]), /*Receiving port address */
							   SharedData);               /*Shared data */
	
	COM_Thread COM             = new COM_Thread("Communication",SharedData,COMint);
	
	COM.start();

    }

}
