/**
 * Flight Management System (FMS)
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

import com.MAVLink.enums.*;
import com.MAVLink.common.*;
import gov.nasa.larcfm.Util.ParameterData;

public class FMS implements Runnable {

	private enum FMS_STATE {
		IDLE, PREFLIGHT, FLIGHT, TERMINATE
	}

	private Thread t;
	private String threadName;
	private Aircraft UAS;

	private FMS_STATE state;
	private boolean FMSrunning;
	private ParameterData pData;

	public FMS(String name, Aircraft ac, ParameterData pdata) {
		threadName = name;
		UAS = ac;
		FMSrunning = true;
		state = FMS_STATE.IDLE;
		pData = pdata;
	}

	public void run() {
		while (FMSrunning) {
			UAS.timeCurrent = System.nanoTime();
			FlightManagement();
		}

		UAS.error.addWarning("[" + UAS.timeLog + "] MSG:FMS shutting down");
	}

	public void start() {
		System.out.println("Starting " + threadName);
		t = new Thread(this, threadName);
		t.start();
	}

	public synchronized boolean isFMSrunning() {
		return FMSrunning;
	}

	public synchronized FMS_STATE getFMSstate() {
		return state;
	}

	public void Reset() {
		state = FMS_STATE.IDLE;
		UAS.Reset();
	}

	public void FlightManagement() {

		int status = 0;

		UAS.FlightData.GetGPSdata();
		UAS.FlightData.GetAttitude();

		synchronized (UAS) {
			if (UAS.IcarousReset) {
				UAS.IcarousReset = false;
				Reset();
			}
		}

		switch (state) {

		case IDLE:

			/* - Wait for mission start flag from ground station */

			if (UAS.FlightData.startMission == 0) {
				if (UAS.FlightData.InputFlightPlan.size() > 0) {
					state = FMS_STATE.PREFLIGHT;
				} else {
					UAS.error.addError("[" + UAS.timeLog + "] MSG: No flight plan loaded");
				}
				UAS.FlightData.startMission = -1;
			} else if (UAS.FlightData.startMission > 0
					&& UAS.FlightData.startMission < UAS.FlightData.InputFlightPlan.size()) {
				if (UAS.FlightData.InputFlightPlan.size() > 0) {
					state = FMS_STATE.FLIGHT;
					UAS.state = Aircraft.FLIGHT_STATE.CRUISE;
					UAS.FlightData.FP_nextWaypoint = UAS.FlightData.startMission;
					UAS.error.addWarning("[" + UAS.timeLog + "] MSG: Starting mission to waypoint:"
							+ UAS.FlightData.FP_nextWaypoint);
				} else {
					UAS.error.addError("[" + UAS.timeLog + "] MSG: No flight plan loaded");
				}
				UAS.FlightData.startMission = -1;
			}

			break;

		case PREFLIGHT:

			/* Peform preflight activities */
			status = UAS.PreFlight();

			if (status == 1) {
				state = FMS_STATE.FLIGHT;
				UAS.state = Aircraft.FLIGHT_STATE.TAKEOFF;
			}

			break;

		case FLIGHT:

			/* Start monitoring mission */
			status = UAS.Flight();

			if (status == 1) {
				state = FMS_STATE.TERMINATE;
			} else if (status == -1) {
				state = FMS_STATE.IDLE;
				UAS.error.addError("[" + UAS.timeLog + "] MSG: FMS state IDLE");
			}
			break;

		case TERMINATE:
			/* Perform post flight activites */

			status = UAS.Terminate();

			break;

		}
	}

}
