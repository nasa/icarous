/**
 * Conflict
 * Contact: Swee Balachandran (swee.balachandran@nianet.org)
 * 
 * 
 * Copyright (c) 2011-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
package gov.nasa.larcfm.ICAROUS;

import java.util.*;
import java.lang.*;

enum CONFLICT_TYPE{
	NONE,KEEP_IN, KEEP_OUT, TRAFFIC, OBSTACLE, EXAMINE, FLIGHTPLAN
}

enum PRIORITY_LEVEL{
    LOW, MEDIUM, HIGH;
}

public class Conflict{   

    CONFLICT_TYPE conflictType;
    PRIORITY_LEVEL priority;

    GeoFence fence;
    GenericObject object;

    public Conflict(PRIORITY_LEVEL level, CONFLICT_TYPE ctype,GeoFence GF){
	priority     = level;
	conflictType = ctype;
	fence        = GF;	
    }

    public Conflict(PRIORITY_LEVEL level, CONFLICT_TYPE ctype,GenericObject OB){
	priority     = level;
	conflictType = ctype;
        object       = OB;
    }

    public void SetPriority(PRIORITY_LEVEL level){
	priority = level;
    }

    public int IsEqual(Conflict conf){

	int val = 0; // -1 - not equal, 0-equal, 1-equal/diff priority
	if(conflictType == conf.conflictType){

	    switch(conflictType){
		
	    case KEEP_IN:
	    case KEEP_OUT:
		if (fence.ID == conf.fence.ID){
		    if(priority == conf.priority)
			return 0;
		    else
			return 1;
		}
		else
		    return -1;
		
	    case TRAFFIC:
	    case OBSTACLE:		
	    case EXAMINE:

		if (object.id == conf.object.id){
		    if(priority == conf.priority)
			return 0;
		    else
			return 1;
		}
		else
		    return -1;
		
	    
	    
	    } // switch
   
	} // if
	return -1;
    }

	    
    public static void AddConflictToList(List<Conflict> conflictList, Conflict conf){

	if(conflictList.size() > 0){
	    for(int i=0;i<conflictList.size();i++){

		Conflict con = (Conflict) conflictList.get(i);

		int check = con.IsEqual(conf);

		if(check < 0)
		    conflictList.add(conf);
		else if(check > 0)
		    con.SetPriority(conf.priority);
	
	    }
	}
	else{
	    conflictList.add(conf);	
	}
	
    }

    public static void RemoveConflicts(){

    }

}
