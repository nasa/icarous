/**
 * ICAROUS class
 *
 * A flight management system for autonomous operation of UAS
 *
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

#ifndef ICAROUS_H_
#define ICAROUS_H_

#include <stdio.h>
#include <thread>
#include <fstream>
#include <string.h>
#include <getopt.h>
#include <string>

#include "Port.h"
#include "FlightManagementSystem.h"
#include "ParameterData.h"
#include "SeparatedInput.h"
#include "Icarous_msg.h"
#include "Interface.h"

class Icarous_t{

private:
	ParameterData paramData;
	AircraftData_t FlightData;
	std::list <geofence_t>tempVertices;
	bool usePlexil;

public:
	Icarous_t(int argc,char* argv[]);
	void GetOptions(int argc,char* argv[]);
	void Initialize();
    void Run(Interface_t* AP,Interface_t* GS);

    // Input interface functions
    void InputStartMission(int param);
    void InputResetIcarous();
    void InputClearFlightPlan();
    void InputParamTable(ParameterData* pData);
	void InputFlightPlanData(waypoint_t* wp);
	void InputGeofenceData(geofence_t* gf);
	void InputPosition(position_t* pos);
	void InputAttitude(attitude_t* att);
	void InputMissionItemReached(missionItemReached_t* msnItem);
	void InputTraffic(object_t* obj);
	void InputAck(CmdAck_t* ack);

    // Output interface functions
	int OutputCommand(ArgsCmd_t* cmd);
	int OutputKinematicBands(visbands_t* bands);
	int OutputGPSPosition(position_t* pos);
    int OutputAttitude(attitude_t* att);
    int OutputWaypoint(waypoint_t* wp);
    int OutputGeofence(geofence_t* gf);

    void OutputToAP(Interface_t* iface);
    void OutputToGS(Interface_t* iface);

	FlightManagementSystem_t *FMS;

	static std::string release();
	static std::string VERSION;
	bool verbose = false;
	bool debug = false;
	char px4port[100];
	char sitlhost[100];
	char gshost[100];
	char gsradio[100];
	char mode[100];
	char config[100];
	int  px4baud = 0;
	int radiobaud = 0;
	int sitlin =0,sitlout =0;
	int gsin =0,gsout = 0;
};



#endif /* ICAROUS_H_ */
