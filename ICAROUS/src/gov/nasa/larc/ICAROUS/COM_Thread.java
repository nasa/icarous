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

enum FP_READ_STATE{
    FP_INFO, FP_WAYPT_INFO, FP_ACK
}


public class COM_Thread implements Runnable{

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
	}

	if(SharedData.RcvdMessages.GeoFenceUpdateInterrupt == 1){


	}

	if(SharedData.RcvdMessages.TrafficUpdateInterrupt == 1){

	}

	if(SharedData.RcvdMessages.ObstacleUpdateInterrupt == 1){


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
	  FP_READ_STATE state1 = FP_INFO;
	  int count            = 0;
	  boolean failure      = false;
	  
	  while(getFP){
	      switch(state1){
		  
	      case FP_IFO:
		  
		  msg_flightplan_info msg1 = SharedData.RcvdMessages.msgFlightplanInfo;

		  SharedData.CurrentFlightPlan.FlightPlanInfo(msg.numWaypoints,msg.maxHorDev,msg.maxVerDev,msg.standoffDist);
		  
		  state1 = WAYPOINT_INFO;
		  
		  break;
		  
	      case FP_WAYPT_INFO:
		  
		  COM.Read();

		  msg_pointofinterest msg2 = SharedData.RcvdMessages.msgPointofinterest;

		  if(msg2.id == 0 && msg2.index != count){
		      failure = true;
		      state1  = ACKNOWLEDGE;
		      break;
		  }
		  
		  Waypoint wp = new Waypoint(msg2.id,msg2.lat,msg2.lon,msg2.alt,msg2.alt);

		  SharedData.CurrentFlightPlan.AddWaypoints(count,wp);

		  if(count == SharedData.CurrentFlightPlan.numWaypoints){
		      state1  = ACKNOWLEDGE;
		      failure = false;
		  }
		  else{
		      count++;
		  }
		  
		  break;
		  
	      case FP_ACK:

		  msg_command_acknowledgement msg3 = new msg_command_acknowledgement();

		  msg3.acktype = 0;
		  
		  if(failure){
		      getFP = false;
		      // Send acknowledgment
		      msg3.value = 0;
		  }
		  else{
		      getFP = false;
		      // Send acknowledgment
		      msg3.value = 1;
		  }
		  
		  COM.Write(msg3);
		  
		  break;
		  
	      }
	  }
	  
    }
    
    
}
