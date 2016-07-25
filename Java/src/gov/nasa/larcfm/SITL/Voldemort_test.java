/**
 * Example program to test VOLDEMORT
 * Contact: Swee Balachandran (swee.balachandran@nianet.org)
 * 
 * 
 * Copyright (c) 2011-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
import gov.nasa.larcfm.ICAROUS.*;
import gov.nasa.larcfm.MISSION.*;
import java.io.*;

public class Voldemort_test{
    
    public static void main(String args[]){
	AircraftData FlightData    = new AircraftData();
	
	
	Interface SITLInt    = new Interface(Interface.SOCKET,
					   null,
					   Integer.parseInt(args[0]),
					   0,
					   FlightData);
	
	Interface COMInt   = new Interface(Interface.SOCKET,
					   null,
					   Integer.parseInt(args[1]),
					   0,
					   FlightData);
	
	
	Demo test = new Demo();
	
	Aircraft uasQuad  = new Aircraft(SITLInt,COMInt,FlightData,test);		
	COM com_module    = new COM("Communications",uasQuad);
	
	com_module.error.setConsoleOutput(true);
	uasQuad.error.setConsoleOutput(true);
	test.error.setConsoleOutput(true);			
	    	
	com_module.start();

	
	
    }

}
