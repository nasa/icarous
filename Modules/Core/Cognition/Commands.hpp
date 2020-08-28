#ifndef COGNITION_COMMANDS_H
#define COGNITION_COMMANDS_H

// Command structure containing data for velocity commands / vector control
typedef struct{
    double vn;          // Speed North (m/s)
    double ve;          // Speed East (m/s)
    double vu;          // Speed Up (m/s)
}VelocityCommand;

// Command structure containing data for a point to point command
typedef struct{
    double point[3];    // Target point lat, lon, alt (deg, deg, m)
    double speed;       // Speed to target point (m/s)
}P2PCommand;

// Command structure containing data for speed changes
typedef struct{
    char name[25];      // Plan ID
    double speed;       // New speed value (m/s)
    int hold;           // 0: use new speed only until next wp, 1: use new speed for all future wps
}SpeedChange;

typedef struct{
    char name[25];
    double altitude;
    int hold;
}AltChange;

// Command structure containing data for takeoff commands
typedef struct{
    char temp;
}TakeoffCommand;

// Command structure containing data for land commands
typedef struct{
    char temp;
}LandCommand;

// Command structure containing data for ditch commands
typedef struct{
    char temp;
}DitchCommand;

// Command structure containing data for flightplan changes
typedef struct{
    char name[25];
    int wpIndex;
    int nextFeasibleWp;
}FpChange;

// Command structure containing data for a new flightplan request
typedef struct{
    int searchType;
    char name[25];
    double fromPosition[3];
    double toPosition[3];
    double startVelocity[3];
}FpRequest;

// Command structure containing data for a new flightplan request
typedef struct{
    int severity;
    char buffer[250];
}StatusMessage;

// Union type for all cognition output commands
typedef struct{
    enum {
        VELOCITY_COMMAND,
        P2P_COMMAND,
        SPEED_CHANGE_COMMAND,
        ALT_CHANGE_COMMAND,
        TAKEOFF_COMMAND,
        LAND_COMMAND,
        DITCH_COMMAND,
        FP_CHANGE,
        FP_REQUEST,
        STATUS_MESSAGE
    } commandType;

    union {
        VelocityCommand velocityCommand;
        P2PCommand p2PCommand;
        SpeedChange speedChange;
        AltChange altChange;
        TakeoffCommand takeoffCommand;
        LandCommand landCommand;
        DitchCommand ditchCommand;
        FpChange fpChange;
        FpRequest fpRequest;
        StatusMessage statusMessage;
    };
}Command;

#endif
