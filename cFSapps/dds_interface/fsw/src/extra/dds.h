#ifndef SBD_OSPL_DDS_H
#define SBD_OSPL_DDS_H

int GPSInfo_DDSInit(void);

void GPSInfo_Send(
        unsigned long id,
        double time_gps,
        double latitude,
        double longitude,
        double altitude_abs,
        double altitude_rel,
        double vx,
        double vy,
        double vz);

#endif
