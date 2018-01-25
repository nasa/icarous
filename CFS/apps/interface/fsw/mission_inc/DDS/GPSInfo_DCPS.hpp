#ifndef OPENSPLICE_ISOCXX_PSM
#define OPENSPLICE_ISOCXX_PSM
#endif
#ifndef CCPP_GPSINFO_H
#define CCPP_GPSINFO_H

#include "ccpp.h"
#include "GPSInfo.h"
#include "GPSInfoDcps.h"
#include <orb_abstraction.h>
#include "GPSInfoDcps_impl.h"

#endif /* CCPP_GPSINFO_H */

#ifdef GPSINFO_DCPS_TYPESUPPORT_DEFINED
#ifndef GPSINFO_DCPS_HPP_
#define GPSINFO_DCPS_HPP_
#include "dds/dds.hpp"

REGISTER_TOPIC_TRAITS(::MAVLink::GPSInfo)

#endif
#endif
