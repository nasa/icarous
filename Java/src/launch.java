/**
 * Example file to run ICAROUS with a sample mission
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

public class launch{
    
    public static void main(String args[]){
		
	Demo test = new Demo();
	Icarous IcarousMain = new Icarous(args,test);
	IcarousMain.run();
	
    }// end of main    
}// end of class
