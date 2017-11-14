#ifndef ICAROUS_ARDUPILOTINTERFACEWITHDDSFORWARDING_HPP
#define ICAROUS_ARDUPILOTINTERFACEWITHDDSFORWARDING_HPP

#include "INTERFACE/MAVLinkInterface.h"
#include "DDS/Common.hpp"

class ArduPilotInterfaceWithDDSForwarding : public ArduPilotInterface_t {
private:
    dds::pub::DataWriter<MAVLink::GPSInfo> gpsInfoWriter;
public:
    ArduPilotInterfaceWithDDSForwarding(Icarous_t *ic);

    void HandlePosition(const mavlink_message_t *message) override;
};

#endif
