/**
 * MAVLink message decoder
 * 
 * This is a class that will decode MAVLink messages
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

#ifndef MAVLINKINBOX_H_
#define MAVLINKINBOX_H_

#include <stdio.h>
#include <pthread.h> // This uses POSIX Threads
#include "common/mavlink.h"
#include <queue>

class MAVLinkInbox{

    private:
        pthread_mutex_t lock;
        mavlink_heartbeat_t heartbeat;
        mavlink_gps_raw_int_t gpsRawInt;
        mavlink_gps_inject_data_t gpsInjectData;

        // Message queues
        std::queue<mavlink_mission_count_t> listMissionCount;
        std::queue<mavlink_mission_item_t> listMissionItem;
        std::queue<mavlink_mission_request_list_t> listMissionRequestList;
        std::queue<mavlink_mission_request_t> listMissionRequest;
        std::queue<mavlink_param_request_list_t> listParamRequestList;
        std::queue<mavlink_param_request_read_t> listParamRequestRead;
        std::queue<mavlink_param_set_t> listParamSet;
        std::queue<mavlink_param_value_t> listParamValue;
        std::queue<mavlink_command_long_t> listCommandLong;
        std::queue<mavlink_command_int_t> listCommandInt;
        std::queue<mavlink_set_mode_t> listSetMode;
        
    public:
        
        MAVLinkInbox();
        void DecodeMessage(mavlink_message_t message);
        bool GetMissionCount(mavlink_mission_count_t& msg);
        bool GetMissionItem(mavlink_mission_item_t& msg);
        bool GetMissionRequestList(mavlink_mission_request_list_t& msg);
        bool GetMissionRequest(mavlink_mission_request_t& msg);
        bool GetParamRequestList(mavlink_param_request_list_t& msg);
        bool GetParamSet(mavlink_param_set_t& msg);
        bool GetParamValue(mavlink_param_value_t& msg);
        bool GetParamRequestRead(mavlink_param_request_read_t& msg);
        bool GetSetMode(mavlink_set_mode_t& msg);
        bool GetCommandLong(mavlink_command_long_t& msg);
        bool GetCommandInt(mavlink_command_int_t& msg);
};

#endif