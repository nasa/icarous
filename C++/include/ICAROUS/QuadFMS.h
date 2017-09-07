/**
 * Quad Flight Management System
 *
 * Core flight management functions for quadrotors
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

#ifndef QUADFMS_H_
#define QUADFMS_H_

#include "FlightManagementSystem.h"
#include "RRT.h"
#include "Daidalus.h"
#include "KinematicMultiBands.h"
#include "BoundingRectangle.h"
#include "DensityGrid.h"
#include "DensityGridAStarSearch.h"
#include <time.h>

class QuadFMS_t:public FlightManagementSystem_t{

	public:
		enum resolve_state_t {IDLE_r, COMPUTE_r, MANEUVER_r, TRAJECTORY_r, RESUME_r};

    private:
		resolve_state_t resolutionState;
		bool newSolution;

    public:

        QuadFMS_t(AircraftData_t* fData);
        ~QuadFMS_t();
        uint8_t TAKEOFF();
        uint8_t CLIMB();
        uint8_t CRUISE();
        uint8_t DESCEND();
        uint8_t LAND();
        uint8_t Monitor();
        uint8_t Resolve();
        uint8_t TRACKING(Position target);

        uint8_t FlyTrajectory(bool newPath);
        uint8_t FlyManeuver(bool newPlan);
        void ComputeInterceptCourse();
        void Reset();
        double SaturateVelocity(double V, double Vsat);
};



#endif /* QUADFMS_H_ */
