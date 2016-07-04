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

import com.MAVLink.icarous.*;


public class COM_Thread implements Runnable{

    public enum FP_READ_STATE{
	FP_INFO, FP_WAYPT_INFO, FP_ACK_FAIL,FP_ACK_SUCCESS
    }

    
    public Thread t;
    public String threadName;
    public AircraftData SharedData;
    public ICAROUS_Interface COM;
    
    public COM_Thread(String name,AircraftData Input, ICAROUS_Interface comInterface){
	threadName       = name;
	SharedData       = Input;
	COM              = comInterface;
    }

    public void run(){

	COM.Read();

	if(SharedData.RcvdMessages.FlightPlanUpdateInterrupt == 1){
	    //TODO: synchronization
	    synchronized(SharedData){
		UpdateFlightPlan();
	    }
	}

	if(SharedData.RcvdMessages.GeoFenceUpdateInterrupt == 1){


	}

	if(SharedData.RcvdMessages.TrafficUpdateInterrupt == 1){

	}

	if(SharedData.RcvdMessages.ObstacleUpdateInterrupt == 1){


	}

	System.out.println("Printing received waypoint information");
	for(int i=0;i<SharedData.CurrentFlightPlan.wayPoints.size();i++){
	    Waypoint wp = (Waypoint) SharedData.CurrentFlightPlan.wayPoints.get(i);
	    System.out.println("**** Waypoint "+i+" ****");
	    System.out.println("latitude " + wp.lat);
	    System.out.println("longitude " + wp.lon);
	}
	

	// TODO: Implement COM box write
	// COM.Write()	

    }

    
    public void start(){
	System.out.println("Starting "+threadName);
	t = new Thread(this,threadName);
	t.start();
    }

	
    public void UpdateFlightPlan(){
	
	  boolean getFP        = true;
	  FP_READ_STATE state1 = FP_READ_STATE.FP_INFO;
	  int count            = 0;
	  
	  while(getFP){
	      switch(state1){
		  
	      case FP_INFO:
		  
		  msg_flightplan_info msg1 = SharedData.RcvdMessages.msgFlightplanInfo;

		  SharedData.CurrentFlightPlan.FlightPlanInfo(msg1.numWaypoints,msg1.maxHorDev,msg1.maxVerDev,msg1.standoffDist);
		  
		  state1 = FP_READ_STATE.FP_WAYPT_INFO;

		  System.out.println("Received flight plan info, Reading " +msg1.numWaypoints+" waypoints");
		  
		  break;
		  
	      case FP_WAYPT_INFO:
		  
		  COM.Read();

		  msg_pointofinterest msg2 = SharedData.RcvdMessages.msgPointofinterest;

		  if(msg2.id == 0 && msg2.index != count){
		      
		      state1  = FP_READ_STATE.FP_ACK_FAIL;
		      break;
		  }
		  
		  Waypoint wp = new Waypoint(msg2.id,msg2.lat,msg2.lon,msg2.alt,msg2.alt);

		  SharedData.CurrentFlightPlan.AddWaypoints(count,wp);

		  if(count == (SharedData.CurrentFlightPlan.numWayPoints-1)){
		      state1  = FP_READ_STATE.FP_ACK_SUCCESS;
		      
		  }
		  else{
		      count++;
		      System.out.println("Receiving next waypoint");
		  }
		  
		  break;

	      case FP_ACK_SUCCESS:

		  msg_command_acknowledgement msg3 = new msg_command_acknowledgement();

		  msg3.acktype = 0;
		  
		 
		  getFP = false;
		  // Send acknowledgment
		  msg3.value = 1;
		  
		  
		  COM.Write(msg3);

		  System.out.println("Waypoints received successfully");
		  
		  break;  
		  
	      case FP_ACK_FAIL:

		  msg_command_acknowledgement msg4 = new msg_command_acknowledgement();

		  msg4.acktype = 0;
		  
		  
		  getFP = false;
		  // Send acknowledgment
		  msg4.value = 0;
		  
		  
		  COM.Write(msg4);
		  
		  break;
		  
	      }
	  }
	  
    }
    
    
}
