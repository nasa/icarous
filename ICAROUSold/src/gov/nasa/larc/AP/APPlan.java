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

import java.sql.Date;
import java.util.ArrayList;

import gov.nasa.larc.ICAROUS.Icarous;
import gov.nasa.larc.serial.Loggable;
import gov.nasa.larcfm.Util.LatLonAlt;
import gov.nasa.larcfm.Util.Plan;
import gov.nasa.larcfm.Util.NavPoint;
import gov.nasa.larcfm.Util.Position;
import gov.nasa.larcfm.Util.Units;

/**
 * @author cquach
 *
 */
public class APPlan implements Loggable
{

//====================================================================================================================//
// DATA Section
//====================================================================================================================//
	public static APWayPoint APhome;

	public String planID = null;
	public ArrayList<APWayPoint> wpList = new ArrayList<APWayPoint>();	
	public long requestTime = 0;
	public long finishTime = 0;
	public int WaypointsInFP = 0;
	public boolean complete = false;	
	
//====================================================================================================================//
// METHOD Section
//====================================================================================================================//

	//=====================================================================================//
	/**
	 * Create a new AP waypoint list for sending to AP.  The source list is a StratWay Plan 
	 * @param newPlan: the Plan object
	 * @param simTime: the time used to find the current segment being flown.  This becomes wp1 in the flight plan  
	 */
	//=====================================================================================//
	public APPlan (Plan newPlan, double simTime) 
	{
		this.requestTime = System.currentTimeMillis();
		planID = newPlan.getName();
		wpList = planToAPPlan ( newPlan, simTime);	
		this.WaypointsInFP =wpList.size();
		setReadyForUse();
		
		this.finishTime = System.currentTimeMillis();

		setReadyForUse();
	}

	//=====================================================================================//
	/**
	 * Create a new AP waypoint list for sending to AP.  The source list is a StratWay Plan 
	 * @param planID: a string to identify the plan
	 * @param newPlan: the Plan object
	 * @param simTime: the time used to find the current segment being flown.  This becomes wp1 in the flight plan  
	 */
	//=====================================================================================//
	public APPlan (String id, Plan newPlan, double simTime) 
	{
		this ( newPlan, simTime);	
		planID = new String(id);
		//wpList = PlanToAPPlan ( newPlan, simTime);	
		//setReadyForUse();
		
	}


	//=====================================================================================//
	/**
	 * Create a new APPlan with the given id and an empty point list.
	 * @param id: a string to identify the plan
	 */
	//=====================================================================================//
	public APPlan(String id) {
		planID = new String(id);
	}


	//=====================================================================================//
	/**
	 * Copy constructor given an array list and ID. 
	 * @param planID: a string to identify the plan
	 * @param newPlan: the APWayPoint list to copy
	 */
	//=====================================================================================//
	public APPlan (APPlan newPlan) 
	{
		planID = new String(newPlan.planID);
		wpList = copyList ( newPlan.wpList);	
		this.WaypointsInFP =newPlan.WaypointsInFP;
		this.requestTime = newPlan.requestTime;
		this.finishTime = newPlan.finishTime;
		setReadyForUse();
	}

	//=====================================================================================//
	/**
	 * Copy constructor given an array list and ID. 
	 * @param planID: a string to identify the plan
	 * @param newPlan: the APWayPoint list to copy
	 */
	//=====================================================================================//
	public APPlan (String id, ArrayList<APWayPoint> newPlan) 
	{
		this.requestTime = System.currentTimeMillis();
		planID = new String(id);
		wpList = copyList ( newPlan);		
		this.WaypointsInFP = wpList.size(); 
		setReadyForUse();
		this.finishTime = System.currentTimeMillis();
	}

	//=====================================================================================//
	/**
	 * Add a waypoint to the plan. 
	 * @param planID: a string to identify the plan
	 * @param newPlan: the APWayPoint list to copy
	 */
	//=====================================================================================//
	public void add (APWayPoint p) 
	{
		if (wpList == null)	{
			wpList = new ArrayList<APWayPoint>();
		}
		wpList.add(p);		
	}

	//=====================================================================================//
	/**
	 * Get the waypoint at index. 
	 * @param idx: index
	 * @return the APwaypoint at idx.
	 */
	//=====================================================================================//
	public APWayPoint add (int i) 
	{
		if ( (wpList != null)
			&& (wpList.size() >= i) )	
		{
			return wpList.get(i);
		}
		return null;		
	}

	//=====================================================================================//
	/**
	 * 
	 * @return the size of the AP plan.
	 */
	//=====================================================================================//
	public void setReadyForUse() 
	{
		complete = true;		
	}

	//=====================================================================================//
	/**
	 * 
	 * @return the size of the AP plan.
	 */
	//=====================================================================================//
	public boolean isReadyForUse() 
	{
		return complete;		
	}

	//=====================================================================================//
	/**
	 * 
	 * @return the size of the AP plan.
	 */
	//=====================================================================================//
	public int size() 
	{
		return wpList.size();		
	}

	//=====================================================================================//
	/**
	 * Sets the time we start getting points from AP
	 */
	//=====================================================================================//
	public void setRequestTime() 
	{
		requestTime = System.currentTimeMillis();		
	}

	//=====================================================================================//
	/**
	 * Sets the time we finish getting points from AP
	 */
	//=====================================================================================//
	public void setFinishTime() 
	{
		finishTime = System.currentTimeMillis();		
	}

	//=====================================================================================//
	/**
	 * @return Elapsed time for getting points from AP
	 */
	//=====================================================================================//
	public long lapseTimeGettingPlan() 
	{
		return finishTime - requestTime;		
	}


	//=====================================================================================//
	/**
	 * Copy the given way point and add a APWayPoint to this APPlan.  Caller must guarantee 
	 * way points are added in sequence.
	 * 
	 * @param APWayPoint
	 */
	//=====================================================================================//
	public void addWayPoint(APWayPoint wp) 
	{
		APWayPoint newWP = new APWayPoint(wp);	// copy to new memory area
		newWP.target_system = (short)1;
//		if ( (lastWPReceived != null)
//			&& (newWP.seq != (lastWPReceived.seq + 1) ) )
//		{
//			// handle out of order insertions here!
//			// also TODO: check for duplicates before adding.  also need to sort waypoints by waypoint seq number.
//		}
//		lastWPReceived = wp;
		wpList.add(newWP);	// wplist contains all waypoints from the AP.
		if (newWP.seq  == 0) {
			APhome = new APWayPoint(newWP);
		}		
	}

	
	//=====================================================================================//
	/**
	 * Convert this APPlan to a Plan object
	 * @return  the header string to write to file
	 */
	//=====================================================================================//
	public Plan toPlan(double entryTimeOffset)
	{
		Plan newPlan = new Plan();
		double cumulativeTime = entryTimeOffset;
		double firstWpTime = -1;
		// note we skip wp(0) which is the Home position in any given AP plan
		// we also skip wp(last) which for the Edge is always a jump back to wp(1)
		APWayPoint wp;
//		int lastPtIdx = wpList.size() - 1;
		for (int i=1; i<wpList.size(); i++)
		{
			wp = wpList.get(i);
			switch (wp.getCommand())  {
			case APWayPoint.WPTYPE_LATLON: 		// its a lat/lon waypoint
				// create a stratway position and add to a stratway points table
				if (firstWpTime < 0) firstWpTime = (double)wp.getTime();
				cumulativeTime += (double)wp.getTime();
				NavPoint np = wp.toNavPoint(cumulativeTime);
				newPlan.add(np);
//				System.out.println(Thread.currentThread().getName()
//						+ "--> APPlan::toPlan(1 param): cumulativeTime ["+cumulativeTime+"] Convert wp["+wp.toString()+"] to NabPoint ["+np+"] Plan..."
//						);

				
				break;
			case APWayPoint.WPTYPE_LOITER: 		// Unlimited Loiter at a lat/lon 
				// ignor this point for now
				break;
			case APWayPoint.WPTYPE_JUMP:	// its a jump command to loop current waypoint back to a previous waypoints
				break;
			default:
				break;
			}	// end of case(p.command)
			
		}
		
		// repeat first point at the end for a looping Plan plan
		NavPoint p = newPlan.point(0);
//		double newTime = p.time();
//		newTime += (double)cumulativeTime;
		newPlan.add(p.makeTime(firstWpTime + cumulativeTime));

		return newPlan;
	}
	
	
	//=====================================================================================//
	/**
	 * Convert this APPlan to a Plan object rotating the first point to the nextWP
	 * @return  the header string to write to file
	 */
	//=====================================================================================//
	public Plan toPlan(double entryTimeOffset, int nextWP)
	{
		Plan newPlan = new Plan();
		double cumulativeTime = entryTimeOffset;
		double firstWpTime = -1;
		int index = nextWP;
		if(nextWP == 1){
			index = wpList.size()-1;
		}
		// note we skip wp(0) which is the Home position in any given AP plan
		// we also skip wp(last) which for the Edge is always a jump back to wp(1)
		APWayPoint wp;
//		int lastPtIdx = wpList.size() - 1;
		for (int i=1; i<wpList.size(); i++)
		{
			wp = wpList.get(index);
			switch (wp.getCommand())  {
//			case APWayPoint.WPTYPE_LOITER: 		// Unlimited Loiter at a lat/lon 
			case APWayPoint.WPTYPE_LATLON: 		// its a lat/lon waypoint
				// create a stratway position and add to a stratway points table
				if (firstWpTime < 0) firstWpTime = (double)wp.getTime();
				cumulativeTime += (double)wp.getTime();
				NavPoint np = wp.toNavPoint(cumulativeTime);
				
				newPlan.add(np);
//				System.out.println(Thread.currentThread().getName()
//						+ "--> APPlan::toPlan: cumulativeTime ["+cumulativeTime+"] Convert wp["+wp.toString()+"] to NabPoint ["+np+"] Plan..."
//						);

				break;
			case APWayPoint.WPTYPE_LOITER: 		// Unlimited Loiter at a lat/lon 
				// ignor this point for now
				break;
			case APWayPoint.WPTYPE_JUMP:	// its a jump command to loop current waypoint back to a previous waypoints
				break;
			default:
				break;
			}	// end of case(p.command)
			index++;
			if(index == wpList.size())
			{
				index = 1;
			}
		}
		
		// repeat first point at the end for a looping Plan plan
		NavPoint p = newPlan.point(0);
//		double newTime = p.time();
//		newTime += (double)cumulativeTime;
		//newPlan.add(p.makeTime(firstWpTime + cumulativeTime));
		newPlan.add(p.makeTime(cumulativeTime));

		// [CAM] Originally, the code used GlobalState.getSimTime(). Is this needed here?
		Position pos = new Position(LatLonAlt.make((double)MavLink_0_9.GLOBAL_POSITION_INT.lat/1.0E7
				, (double)MavLink_0_9.GLOBAL_POSITION_INT.lon/1.0E7
				, (double)(Units.to("ft",MavLink_0_9.GLOBAL_POSITION_INT.alt/1000.0)) ));
		newPlan.add(new NavPoint(pos,Icarous.getSimTime()));

		return newPlan;
	}
	
	
	//=====================================================================================//
	/**
	 * Create a Plan object with NavPoints from the given AP Plan.  An AP Plan is an
	 * arraylist of APWayPoint objects.  Converts lapseTime in the AP waypoints to cumulative
	 * time in the Plan.  Skip the home and jump points in the AP plan.
	 */
	//=====================================================================================//
//	private Plan APPlanToPlan(ArrayList<APWayPoint> applan, double entryTimeOffset) {
//		Plan newPlan = new LinearPlan();
//		double prevTime = entryTimeOffset;
//		int lastPtIdx = applan.size() - 1;
//		for (int i=1; i<lastPtIdx; i++)
//		{
//			newPlan.add(applan.get(i).toNavPoint(prevTime));
//		}
//		
//		return newPlan;
//	}

	
	
	//=====================================================================================//
	/**
	 * Create a new AP waypoint list for sending to AP.  The source list is a StratWay Plan 
	 * which can either be the trajectory plan with TCP turn points or a linear plan with 
	 * no TCP turn points.  Note that the AP can be running with or without turn anticipation.
	 * Should use the linear plan if the AP has turn anticipation enabled.  Should use the 
	 * trajectory if AP has turn anticipation disabled.
	 * Assumptions:
	 * The AP Home point in this object is assumed to exist before calling this function.
	 * @return a new AP waypoint list  
	 */
	//=====================================================================================//
	public ArrayList<APWayPoint> planToAPPlan ( Plan newPlan, double simTime)
	{

//		System.out.println(Thread.currentThread().getName()
//				+ "--> APPlan::Const(Plan): Converting ["+newPlan.size()+"] point Plan..."
//				);
		// create storage for new AP plan
		ArrayList<APWayPoint> newAPPlan = new ArrayList<APWayPoint>();
		
		// get the  waypoint count in the Plan object
		int CDnRPlanSize = newPlan.size();
		
		// get the current waypoint in the Plan object
		int nextTargetIdx = newPlan.getSegment(simTime) +1;
		if (nextTargetIdx <= 0)	// we have a problem
		{
			nextTargetIdx = 1;
		}
		
		// load the HOME position to the new AP plan
		newAPPlan.add(APPlan.APhome);
		
		// load in first waypoint whose elapse time is from the current time to the given arrival time 
		short seq = 1;
		float lapseTime = (float) (newPlan.point(nextTargetIdx).time() - simTime );
		//short isCurrentWP = 1;
		newAPPlan.add( new APWayPoint(seq++, newPlan.point(nextTargetIdx), (float) lapseTime, (short) 0) );	
		
		for (int i = (nextTargetIdx+1); i<CDnRPlanSize; i++)
		{
		newAPPlan.add( new APWayPoint(seq++, newPlan.point(i), newPlan.point(i-1))  ); 
		}
		
		for (int i = 1; i<nextTargetIdx; i++)
		{
		newAPPlan.add( new APWayPoint(seq++, newPlan.point(i), newPlan.point(i-1))  ); 
		}
		
		// add the jump command
		float jumpTo = (float)1;
		float iterations = (float)-1;
		newAPPlan.add( new APWayPoint(seq, jumpTo, iterations) );	
		
		return newAPPlan;
	}
	

	
	//=====================================================================================//
	/**
	 * Return the APWaypoints of the given index from given APPlan point list.  
	 * 
	 * @param wpCount: the index
	 * @return  the APWaypoints at index.
	 */
	//=====================================================================================//
	public APWayPoint get(int index) {
		
		return wpList.get(index);
	}


	
	//=====================================================================================//
	/**
	 * Copy APWaypoints from given list and return the new list.  
	 * 
	 * @param o: the list to copy
	 * @return  a separate memory copy.
	 */
	//=====================================================================================//
	public ArrayList<APWayPoint> copyList(ArrayList<APWayPoint> o)
	{
		ArrayList<APWayPoint> c = new ArrayList<APWayPoint>();
		for (APWayPoint wp: o)
		{
			c.add(new APWayPoint(wp));
		}
		
		return c;
	
	}
	

	//=====================================================================================//
	/**
	 * Convenience function to return a string representation of all waypoints in an AP 
	 * flight plan array list.
	 * @return true if there are waypoints equal to the number that the AP has.  
	 */
	//=====================================================================================//
	public String printPlan() 
	{
		
		String planString = "-----------------------------------------------------\n";
		planString += "["+planID+":"+wpList.size()+" way points ]:\n";
		for (APWayPoint wp: wpList)
		{
			planString += wp.toString() + "\n";
		}
		planString += "-----------------------------------------------------\n";
		return planString;
	}

		
	
	
	//=====================================================================================//
	/**
	 * @return  the header string to write to file
	 */
	//=====================================================================================//
	public String toString()
	{
		Date date = new Date(System.currentTimeMillis());
		String timeStr = Icarous.timeFormat.format(date);

		String planString = "\n---- "+ timeStr + " ---- \n";
		//planString += ", "+wpList.size();
		planString += planID+ ", WayPoint Count ["+wpList.size()+"]\n";
		for (APWayPoint wp: wpList)
		{
			//planString += ", "+wp.toString();
			planString += wp.toString()+"\n";
		}
		return planString;
	
		
		
	}
	
	//=====================================================================================//
	/**
	 * @return  the header string to write to file
	 */
	//=====================================================================================//
	public String getLogHeader()
	{
		return null;
	
	}
	
	//=====================================================================================//
	/**
	 * @return  the data string to write to file
	 */
	//=====================================================================================//
	public String getLogData ()
	{
		return toString();
		
	}

	
	
//====================================================================================================================//
// MAIN
//====================================================================================================================//
	/**
	 * @param args
	 */
	public static void main(String[] args) {
		// TODO Auto-generated method stub

	}


}	// end APPlan class
