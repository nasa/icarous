#include "GPSInfoSplDcps.h"
#include "GPSInfo_DCPS.hpp"

#include <v_copyIn.h>
#include <v_topic.h>
#include <os_stdlib.h>
#include <string.h>
#include <os_report.h>

v_copyin_result
__MAVLink_GPSInfo__copyIn(
    c_base base,
    const class ::MAVLink::GPSInfo *from,
    struct _MAVLink_GPSInfo *to)
{
    v_copyin_result result = V_COPYIN_RESULT_OK;
    (void) base;

    to->time = (c_double)from->time_;
    to->latitude = (c_double)from->latitude_;
    to->longitude = (c_double)from->longitude_;
    to->absoluteAltitude = (c_double)from->absoluteAltitude_;
    to->relativeAltitude = (c_double)from->relativeAltitude_;
    to->xVelocity = (c_double)from->xVelocity_;
    to->yVelocity = (c_double)from->yVelocity_;
    to->zVelocity = (c_double)from->zVelocity_;
    return result;
}

void
__MAVLink_GPSInfo__copyOut(
    const void *_from,
    void *_to)
{
    const struct _MAVLink_GPSInfo *from = (const struct _MAVLink_GPSInfo *)_from;
    class ::MAVLink::GPSInfo *to = (class ::MAVLink::GPSInfo *)_to;
    to->time_ = (::DDS::Double)from->time;
    to->latitude_ = (::DDS::Double)from->latitude;
    to->longitude_ = (::DDS::Double)from->longitude;
    to->absoluteAltitude_ = (::DDS::Double)from->absoluteAltitude;
    to->relativeAltitude_ = (::DDS::Double)from->relativeAltitude;
    to->xVelocity_ = (::DDS::Double)from->xVelocity;
    to->yVelocity_ = (::DDS::Double)from->yVelocity;
    to->zVelocity_ = (::DDS::Double)from->zVelocity;
}

