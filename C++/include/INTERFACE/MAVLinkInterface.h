/**
 * MAVLink interface for ICAROUS
 *
 * This is a class that will decode/encode, send and receive MAVLink messages
 * and will also store the data in the ICAROUS data structures.
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
#ifndef C_MAVLINKINTERFACE_H
#define C_MAVLINKINTERFACE_H

#include "icarous/mavlink.h"
#include "Interface.h"
#include "Port.h"
#include <ctime>

// Auto Pilot Modes enumeration
enum control_mode_t {
    STABILIZE =     0,  // manual airframe angle with manual throttle
    ACRO =          1,  // manual body-frame angular rate with manual throttle
    ALT_HOLD =      2,  // manual airframe angle with automatic throttle
    AUTO =          3,  // fully automatic waypoint control using mission commands
    GUIDED =        4,  // fully automatic fly to coordinate or fly at velocity/direction using GCS immediate commands
    LOITER =        5,  // automatic horizontal acceleration with automatic throttle
    RTL =           6,  // automatic return to launching point
    CIRCLE =        7,  // automatic circular flight with automatic throttle
    LAND =          9,  // automatic landing with horizontal position control
    DRIFT =        11,  // semi-automous position, yaw and throttle control
    SPORT =        13,  // manual earth-frame angular rate control with manual throttle
    FLIP =         14,  // automatically flip the vehicle on the roll axis
    AUTOTUNE =     15,  // automatically tune the vehicle's roll and pitch gains
    POSHOLD =      16,  // automatic position hold with manual override, with automatic throttle
    BRAKE =        17,  // full-brake using inertial/GPS system, no pilot input
    THROW =        18,  // throw to launch mode using inertial/GPS system, no pilot input
    AVOID_ADSB =   19,  // automatic avoidance of obstacles in the macro scale - e.g. full-sized aircraft
    GUIDED_NOGPS = 20,  // guided mode but only accepts attitude and altitude
};

class MAVLinkInterface_t: public Interface_t{
protected:
    Port_t* PT;
    char portname[50];
    char hostname[50];
    int baudrate;
    int inputport;
    int outputport;

    MAVLinkInterface_t *pipe;

public:
    int* waypointType;
    int numWaypoints;
    MAVLinkInterface_t(Icarous_t* ic);
    ~MAVLinkInterface_t();

    virtual void GetData();
    virtual void SendData(msgType _type, void* data);

    void ConfigurePorts(char* portname,int baudrate);
    void ConfigurePorts(char* hostname,int input,int output);
    void SetPipe(MAVLinkInterface_t* p){pipe = p;};
    void SendMessage(const mavlink_message_t*);
    virtual void SendCommands(ArgsCmd_t*){};
    virtual void SendBands(visbands_t* bands){}; // For visualization in GS
    virtual void ProcessMessage(mavlink_message_t*);
    virtual void HandlePosition(const mavlink_message_t *message){};
    virtual void HandleCommandAck(const mavlink_message_t *message){};
    virtual void HandleMissionItemReached(const mavlink_message_t *message){};
    virtual void HandleMissionRequestList(const mavlink_message_t *message){};
    virtual void HandleMissionRequest(const mavlink_message_t *message){};
    virtual void HandleParamRequestList(const mavlink_message_t *message){};
    virtual void HandleParamRequestRead(const mavlink_message_t *message){};
    virtual void HandleParamValue(const mavlink_message_t *message){};
    virtual void HandleParamSet(const mavlink_message_t *message){};
    virtual void HandleCommandLong(const mavlink_message_t *message){};
    virtual void HandleCommandInt(const mavlink_message_t *message){};
    virtual void HandleSetMode(const mavlink_message_t *message){};
    virtual void HandleRadio(const mavlink_message_t *message){};
    virtual void HandleRadioStatus(const mavlink_message_t *message){};
    virtual void HandleHeartbeat(const mavlink_message_t *message){};
    virtual void HandleMissionCount(const mavlink_message_t *message){};
    virtual void HandleMissionItem(const mavlink_message_t *message){};
    virtual void HandleMissionStart(const mavlink_command_long_t &msg){};
    virtual void HandleFenceEnable(const mavlink_command_long_t &msg){};
    virtual void HandleTraffic(const mavlink_command_long_t &msg){};
    virtual void HandleReset(const mavlink_command_long_t &msg){};
    virtual void HandleFencePoint(const mavlink_message_t *message){};

};


class ArduPilotInterface_t: public MAVLinkInterface_t{
private:
    time_t lastPing;
    time_t newPing;
    double elapsedTime;
    bool foundVehicle;
public:
    ArduPilotInterface_t(Icarous_t* ic);
    void GetData();
    void HandleHeartbeat(const mavlink_message_t *message);
    void HandlePosition(const mavlink_message_t *message);
    void HandleCommandAck(const mavlink_message_t *message);
    void HandleMissionItemReached(const mavlink_message_t *message);

    void SendCommands(ArgsCmd_t*);
};

class MAVProxyInterface_t:public MAVLinkInterface_t{

private:
    geofence_t gfdata;
    double speed;
public:
    MAVProxyInterface_t(Icarous_t* ic);

    void HandleMissionRequestList(const mavlink_message_t *message);
    void HandleMissionRequest(const mavlink_message_t *message);
    void HandleParamRequestList(const mavlink_message_t *message);
    void HandleParamRequestRead(const mavlink_message_t *message);
    void HandleParamValue(const mavlink_message_t *message);
    void HandleParamSet(const mavlink_message_t *message);
    void HandleCommandLong(const mavlink_message_t *message);
    void HandleCommandInt(const mavlink_message_t *message);
    void HandleSetMode(const mavlink_message_t *message);
    void HandleRadio(const mavlink_message_t *message);
    void HandleRadioStatus(const mavlink_message_t *message);
    void HandleMissionStart(const mavlink_command_long_t &msg);
    void HandleFenceEnable(const mavlink_command_long_t &msg);
    void HandleTraffic(const mavlink_command_long_t &msg);
    void HandleReset(const mavlink_command_long_t &msg);
    void HandleFencePoint(const mavlink_message_t *message);
    void HandleMissionCount(const mavlink_message_t *message);
    void HandleMissionItem(const mavlink_message_t *message);

    void SendBands(visbands_t* bands);
};

#endif //C_MAVLINKINTERFACE_H
