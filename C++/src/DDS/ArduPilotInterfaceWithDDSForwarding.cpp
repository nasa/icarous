#include "DDS/ArduPilotInterfaceWithDDSForwarding.hpp"
#include "Icarous.h"

ArduPilotInterfaceWithDDSForwarding::ArduPilotInterfaceWithDDSForwarding(Icarous_t *ic) :
        gpsInfoWriter(dds::core::null),
        ArduPilotInterface_t(ic) {
    gpsInfoWriter = DataWriterFactory<MAVLink::GPSInfo>();
}

void ArduPilotInterfaceWithDDSForwarding::HandlePosition(const mavlink_message_t *message) {
    mavlink_global_position_int_t globalPositionInt;
    mavlink_msg_global_position_int_decode(message, &globalPositionInt);

    position_t position;
    position.time_gps = (double) globalPositionInt.time_boot_ms / 1E3;
    position.latitude = (double) globalPositionInt.lat / 1E7;
    position.longitude = (double) globalPositionInt.lon / 1E7;
    position.altitude_abs = (double) globalPositionInt.alt / 1E3;
    position.altitude_rel = (double) globalPositionInt.relative_alt / 1E3;
    position.vx = (double) globalPositionInt.vx / 100;
    position.vy = (double) globalPositionInt.vy / 100;
    position.vz = (double) globalPositionInt.vz / 100;

    MAVLink::GPSInfo gpsMsg(
            position.time_gps,
            position.latitude,
            position.longitude,
            position.altitude_abs,
            position.altitude_rel,
            position.vx,
            position.vy,
            position.vz);
    this->gpsInfoWriter << gpsMsg;

    icarous->InputPosition(&position);
}
