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
#include <string.h>
#include "icarous_msg.h"
#include "icarous_table.h"
#ifdef __cplusplus
#include <thread>
#include <fstream>
#include <string>

#include "QuadFMS.h"
#include "Geofence.h"

using namespace std;
using namespace larcfm;

class Icarous_t{

private:
	AircraftData_t FlightData;
	list <geofence_t>tempVertices;
	QuadFMS_t FMS;

public:
	Icarous_t(){};
	Icarous_t(icarous_table_t* pData);
	void Run();

	// Function to set input data to ICAROUS
	void SetFlightPlanData(waypoint_t* wp);  // Add flight plan data
	void SetGeofenceData(geofence_t* gf);
	void StartMission(int param);
	int GetCommand(ArgsCmd_t* cmd);
	void InputAck(CmdAck_t* ack);
	void InputPosition(position_t* pos);
	void InputAttitude(attitude_t* att);
	void InputMissionItemReached(missionItemReached_t* msnItem);
	void InputTraffic(object_t* traffic);
	int SendKinematicBands(visbands_t* bands);
	static std::string release();
	static std::string VERSION;
};
#endif /* __cplusplus */

#ifdef __cplusplus
extern "C" {
#endif
    struct icarous_t; // a nice opaque type
    struct icarous_t *icarous_create_init(icarous_table_t* pData);
    void icarous_destroy(struct icarous_t *ic);

    void icarous_setFlightData(struct icarous_t* ic,waypoint_t* wp);
    void icarous_setGeofenceData(struct icarous_t* ic,geofence_t* gfdata);
    void icarous_StartMission(struct icarous_t *ic,float param);
    int icarous_GetCommand(struct icarous_t *ic, ArgsCmd_t* cmd);
    void icarous_run(struct icarous_t* ic);
    void icarous_inputAck(struct icarous_t* ic,CmdAck_t* ack);
    void icarous_inputPosition(struct icarous_t* ic,position_t* pos);
    void icarous_inputAttitude(struct icarous_t* ic,attitude_t* att);
    void icarous_inputMissionItemReached(struct icarous_t* ic,missionItemReached_t* msnItem);
    void icarous_inputTraffic(struct icarous_t* ic,object_t* traffic);
    int icarous_sendKinematicBands(struct icarous_t* ic,visbands_t* bands);
    int32_t ICAROUS_LibInit(void);

#ifdef __cplusplus
}
#endif


#endif /* ICAROUS_H_ */
