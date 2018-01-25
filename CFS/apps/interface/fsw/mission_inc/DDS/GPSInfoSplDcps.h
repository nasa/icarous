#ifndef GPSINFOSPLTYPES_H
#define GPSINFOSPLTYPES_H

#include "GPSInfo_DCPS.hpp"

#include <c_base.h>
#include <c_misc.h>
#include <c_sync.h>
#include <c_collection.h>
#include <c_field.h>

extern c_metaObject __GPSInfo_MAVLink__load (c_base base);

extern c_metaObject __MAVLink_GPSInfo__load (c_base base);
extern const char * __MAVLink_GPSInfo__keys (void);
extern const char * __MAVLink_GPSInfo__name (void);
struct _MAVLink_GPSInfo ;
extern  c_bool __MAVLink_GPSInfo__copyIn(c_base base, class MAVLink::GPSInfo *from, struct _MAVLink_GPSInfo *to);
extern  void __MAVLink_GPSInfo__copyOut(void *_from, void *_to);
struct _MAVLink_GPSInfo {
    c_double time;
    c_double latitude;
    c_double longitude;
    c_double absoluteAltitude;
    c_double relativeAltitude;
    c_double xVelocity;
    c_double yVelocity;
    c_double zVelocity;
};

#endif
