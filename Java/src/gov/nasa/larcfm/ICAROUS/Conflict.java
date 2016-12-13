/**
 * Conflict
 * Contact: Swee Balachandran (swee.balachandran@nianet.org)
 * 
 * 
 * Copyright (c) 2011-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 *
 * Notices:
 *  Copyright 2016 United States Government as represented by the Administrator of the National Aeronautics and Space Administration. 
 *  All rights reserved.
 *     
 * Disclaimers:
 *  No Warranty: THE SUBJECT SOFTWARE IS PROVIDED "AS IS" WITHOUT ANY WARRANTY OF ANY KIND, EITHER EXPRESSED, 
 *  IMPLIED, OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, ANY WARRANTY THAT THE SUBJECT SOFTWARE WILL CONFORM TO SPECIFICATIONS, ANY
 *  IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, OR FREEDOM FROM INFRINGEMENT, 
 *  ANY WARRANTY THAT THE SUBJECT SOFTWARE WILL BE ERROR FREE, OR ANY WARRANTY THAT DOCUMENTATION, IF PROVIDED, 
 *  WILL CONFORM TO THE SUBJECT SOFTWARE. THIS AGREEMENT DOES NOT, IN ANY MANNER, CONSTITUTE AN ENDORSEMENT BY GOVERNMENT 
 *  AGENCY OR ANY PRIOR RECIPIENT OF ANY RESULTS, RESULTING DESIGNS, HARDWARE, SOFTWARE PRODUCTS OR ANY OTHER APPLICATIONS 
 *  RESULTING FROM USE OF THE SUBJECT SOFTWARE.  FURTHER, GOVERNMENT AGENCY DISCLAIMS ALL WARRANTIES AND 
 *  LIABILITIES REGARDING THIRD-PARTY SOFTWARE, IF PRESENT IN THE ORIGINAL SOFTWARE, AND DISTRIBUTES IT "AS IS."
 *
 * Waiver and Indemnity:  
 *   RECIPIENT AGREES TO WAIVE ANY AND ALL CLAIMS AGAINST THE UNITED STATES GOVERNMENT, 
 *   ITS CONTRACTORS AND SUBCONTRACTORS, AS WELL AS ANY PRIOR RECIPIENT.  IF RECIPIENT'S USE OF THE SUBJECT SOFTWARE 
 *   RESULTS IN ANY LIABILITIES, DEMANDS, DAMAGES,
 *   EXPENSES OR LOSSES ARISING FROM SUCH USE, INCLUDING ANY DAMAGES FROM PRODUCTS BASED ON, OR RESULTING FROM, 
 *   RECIPIENT'S USE OF THE SUBJECT SOFTWARE, RECIPIENT SHALL INDEMNIFY AND HOLD HARMLESS THE UNITED STATES GOVERNMENT, 
 *   ITS CONTRACTORS AND SUBCONTRACTORS, AS WELL AS ANY PRIOR RECIPIENT, TO THE EXTENT PERMITTED BY LAW.  
 *   RECIPIENT'S SOLE REMEDY FOR ANY SUCH MATTER SHALL BE THE IMMEDIATE, UNILATERAL TERMINATION OF THIS AGREEMENT.
 */
package gov.nasa.larcfm.ICAROUS;

import java.util.*;
import java.lang.*;
import gov.nasa.larcfm.Util.Position;

enum CONFLICT_TYPE {
	NONE, KEEP_IN, KEEP_OUT, TRAFFIC, OBSTACLE, EXAMINE, FLIGHTPLAN
}

enum PRIORITY_LEVEL {
	LOW, MEDIUM, HIGH;
}

public class Conflict {

	CONFLICT_TYPE conflictType;
	PRIORITY_LEVEL priority;

	GeoFence fence;
	GenericObject object;
	Position place;

	public Conflict(PRIORITY_LEVEL level, CONFLICT_TYPE ctype, GeoFence GF, Position pos) {
		priority = level;
		conflictType = ctype;
		fence = GF;
		place = pos;
	}

	public Conflict(PRIORITY_LEVEL level, CONFLICT_TYPE ctype, GenericObject OB) {
		priority = level;
		conflictType = ctype;
		object = OB;
	}

	public Conflict(PRIORITY_LEVEL level, CONFLICT_TYPE ctype) {
		priority = level;
		conflictType = ctype;
	}

	public void SetPriority(PRIORITY_LEVEL level) {
		priority = level;
	}

	public int IsEqual(Conflict conf) {

		int val = 0; // -1 - not equal, 0-equal, 1-equal/diff priority
		if (conflictType == conf.conflictType) {

			switch (conflictType) {

			case FLIGHTPLAN:
				return 1;

			case KEEP_IN:
				if (fence.ID == conf.fence.ID) {
					double distance = place.distanceH(conf.place);
					if (distance < 5) {
						return 1;
					} else {
						return 0;
					}
				} else {
					return 0;
				}

			case KEEP_OUT:
				if (fence.ID == conf.fence.ID) {
					return 1;
				} else {
					return 0;
				}

			case TRAFFIC:
				return 1;

			case OBSTACLE:
			case EXAMINE:

				if (object.id == conf.object.id) {
					return 0;
				} else {
					return 1;
				}

			} // switch

		} // if
		return -1;
	}

	public void printConflict() {
		System.out.println("CONFLICT TYPE:" + conflictType);
	}

	public static void printConflictList(List<Conflict> conflictList) {
		for (int i = 0; i < conflictList.size(); i++) {
			conflictList.get(i).printConflict();
		}
	}

	public static void AddConflictToList(List<Conflict> conflictList, Conflict conf) {

		if (conflictList.size() > 0) {
			boolean inList = false;
			for (int i = 0; i < conflictList.size(); i++) {

				Conflict con = (Conflict) conflictList.get(i);

				int check = con.IsEqual(conf);

				if (check > 0) {
					inList = true;
				}

			}

			if (!inList) {
				conflictList.add(conf);
				System.out.println("adding conflict");
			}
		} else {
			conflictList.add(conf);
			System.out.println("adding conflict");
		}

	}

	public static void RemoveTrafficConflict(List<Conflict> conflictList) {

		for (int i = 0; i < conflictList.size(); i++) {
			Conflict conf = conflictList.get(i);

			if (conf.conflictType == CONFLICT_TYPE.TRAFFIC) {
				conflictList.remove(i);
			}
		}

	}

	/*
	 * public static void RemoveConflict(List<Conflict> conflictList, Conflict
	 * conf){ if(conflictList.size() > 0){ boolean inList = false; for(int
	 * i=0;i<conflictList.size();i++){
	 * 
	 * Conflict con = (Conflict) conflictList.get(i);
	 * 
	 * int check = con.IsEqual(conf,true);
	 * 
	 * if(check > 0){ conflictList.remove(i);
	 * System.out.println("Removing conflict");
	 * System.out.println("Conflict size:"+conflictList.size()); }
	 * 
	 * }
	 * 
	 * }
	 * 
	 * }
	 */

}
