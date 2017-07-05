/**
 * Flight Management System
 * 
 * Core flight management functions
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

 #ifndef FMS_H
 #define FMS_H

 #include "math.h"
 #include "AircraftData.h"
 #include "NavPoint.h"
 #include "PlanUtil.h"
 #include <fstream>
 #include <iostream>
 #include "icarous_msg.h"

enum fms_state_t {_idle_,_takeoff_,_climb_,_cruise_,_descend_,_land_};

class FlightManagementSystem_t{

    protected:

        fms_state_t fmsState;
        uint8_t conflictSize;
        bool deviationApproved;
        bool landStarted;
        bool icarousActive;
        double sendTkCommand;

    public:


        icarous_control_mode_t currentMode;
        AircraftData_t* FlightData;


        FlightManagementSystem_t(){};
        FlightManagementSystem_t(AircraftData_t* fdata);
        virtual ~FlightManagementSystem_t(){};
        void RunFMS();

        void SendCommand(uint8_t target_system,uint8_t target_component,uint16_t command,uint8_t confirmation,
                         float param1, float param2, float param3,float param4, 
                         float param5, float param6, float param7);

        void SetYaw(bool relative,double heading);
        void SetGPSPos(double lat,double lon, double alt);
        void SetVelocity(double Vn,double Ve,double Vu);
        void SetMode(icarous_control_mode_t mode);
        void SetSpeed(float speed);
        void SendStatusText(char buffer[]);
        void ArmThrottles(bool arm);
        void StartTakeoff(float alt);
        void StartLand();
        void GetLatestAircraftData();
        bool CheckAck(command_name_t command);
        bool CheckMissionWaypointReached();
        double GetApproxElapsedPlanTime(Plan fp, int nextWP);
        void CheckReset();
        void SetDeviationApproved(bool status);
        void SetMissionItem(uint8_t nextWP);
        void GetCurrentMode();

        uint8_t IDLE();
        uint8_t PREFLIGHT();
        virtual uint8_t TAKEOFF(){return 0;};
        virtual uint8_t CLIMB(){return 0;};
        virtual uint8_t CRUISE(){return 0;};
        virtual uint8_t DESCEND(){return 0;};
        virtual uint8_t APPROACH(){return 0;};
        virtual uint8_t LAND(){return 0;};
        virtual uint8_t TRACKING(Position target){return 0;};
        virtual void Reset(){return;};
};



 #endif
