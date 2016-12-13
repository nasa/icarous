/**
 * GenericObject class
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
import com.MAVLink.icarous.*;
import gov.nasa.larcfm.Util.Position;
import gov.nasa.larcfm.Util.Velocity;

public class GenericObject {

	public int id;
	public int type;
	public Position pos;
	public Velocity vel;
	public double orientation;

	public GenericObject(int type_in, int id_in, float lat_in, float lon_in, float altmsl_in, float vx_in, float vy_in,
			float vz_in) {
		id = id_in;
		type = type_in;
		pos = Position.makeLatLonAlt(lat_in, "degree", lon_in, "degree", altmsl_in, "m");
		vel = Velocity.makeVxyz(vy_in, vx_in, "m/s", vz_in, "m/s");
		orientation = vel.trk();

	}

	public boolean isEqual(GenericObject obj, boolean update) {

		if (id == obj.id) {
			if (update) {
				pos = obj.pos.copy();
				vel = obj.vel.mkAddTrk(0);
				orientation = obj.orientation;
			}
			return true;
		} else {
			return false;
		}

	}

	public static void AddObject(List<GenericObject> objectList, GenericObject obj) {

		boolean inList = false;
		for (int i = 0; i < objectList.size(); i++) {
			if (objectList.get(i).isEqual(obj, true)) {
				inList = true;
				break;
			}
		}

		if (!inList) {
			objectList.add(obj);
			System.out.println("Added object");
		}

	}

	public static void RemoveObject(List<GenericObject> objectList, int id) {

		GenericObject obj;
		Iterator Itr = objectList.iterator();

		while (Itr.hasNext()) {
			obj = (GenericObject) Itr.next();

			if (obj.id == id) {
				Itr.remove();
				break;
			}
		}
	}

}
