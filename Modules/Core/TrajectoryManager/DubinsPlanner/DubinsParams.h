#ifndef RRT_PARAMS_H
#define RRT_PARAMS_H

typedef struct DubinsParams{
    /* well clear volume */
    double wellClearDistH;
    double wellClearDistV;

    /* Aircraft constraints */
    double turnRate;
    double gs;
    double vs;
    double maxGS;
    double minGS;
    double maxVS;
    double minVS;
    double hAccel;
    double hDaccel;
    double vAccel;
    double vDaccel;
    double climbgs;

    /* Fence parameters */
    double zSections;
    double vertexBuffer; 
    double maxH;
}DubinsParams_t;



#endif