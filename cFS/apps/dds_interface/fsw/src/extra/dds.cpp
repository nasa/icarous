#ifdef WITH_DDS

#include <string>
#include <cstring>
#include <iostream>
#include <sstream>
#include <cstdint>
#include "ccpp_GPSInfo.h"
#include "ccpp_dds_dcps.h"

extern "C" {
#include <osapi.h>
#include <common_types.h>
#include <cfe_sb.h>
#include <cfe_es.h>
#include <cfe_psp.h>
#include <cfe_error.h>
#include <cfe_sb_events.h>
#include "dds.h"
}

#define DEBUG_MSG(eid, etype, fmt, ...) do \
{ \
  uint32 _task_id = OS_TaskGetId(); \
  OS_printf("%s: " fmt "\n", __func__, ##__VA_ARGS__); \
} while (0);
#define PARTICIPANT_QOS_DEFAULT (*::DDS::OpenSplice::Utils::FactoryDefaultQosHolder::get_domainParticipantQos_default())

using namespace MAVLink;

static DDS::DomainId_t domain_id = 100;
static DDS::DomainParticipant_var participant = NULL;
static DDS::Publisher_var pub = DDS::Publisher::_nil();
static DDS::Subscriber_var sub = DDS::Subscriber::_nil();
static GPSInfoDataWriter_var writer_gpsinfo = GPSInfoDataWriter::_nil();
static GPSInfoTypeSupport_var type_support_gpsinfo = GPSInfoTypeSupport::_nil();


int GPSInfo_DDSInit(void) {
    try {
        // Initialize and create a DomainParticipant
        DDS::DomainParticipantFactory_var dpf = DDS::DomainParticipantFactory::get_instance();
        if (dpf.in() == NULL) {
            DEBUG_MSG(CFE_SB_DDS_ERR_EID, CFE_EVS_ERROR,
                      "create_factory failed.");
            return 1;
        }
        participant = dpf->create_participant(domain_id, PARTICIPANT_QOS_DEFAULT,
                                              NULL,
                                              DDS::STATUS_MASK_NONE);
        if (participant.in() == NULL) {
            DEBUG_MSG(CFE_SB_DDS_ERR_EID, CFE_EVS_ERROR,
                      "create_participant failed.");
            return 2;
        }

        // Register type support
        type_support_gpsinfo = new GPSInfoTypeSupport();
        if (type_support_gpsinfo->register_type(participant.in(), type_support_gpsinfo->get_type_name()) !=
            DDS::RETCODE_OK) {
            DEBUG_MSG(CFE_SB_DDS_ERR_EID, CFE_EVS_ERROR,
                      "Registering type support failed.");
            return 3;
        }

        // Create a publisher for the SB
        pub = participant->create_publisher(PUBLISHER_QOS_DEFAULT, 0,
                                            DDS::STATUS_MASK_NONE);
        if (pub.in() == NULL) {
            DEBUG_MSG(CFE_SB_DDS_ERR_EID, CFE_EVS_ERROR,
                      "create_publisher failed.");
            return 4;
        }

        // Create a subscriber for the SB
        sub = participant->create_subscriber(SUBSCRIBER_QOS_DEFAULT,
                                             DDS::SubscriberListener::_nil(), DDS::STATUS_MASK_NONE);
        if (sub.in() == NULL) {
            DEBUG_MSG(CFE_SB_DDS_ERR_EID, CFE_EVS_ERROR,
                      "create_subscriber failed.");
            return 5;
        }

        // Find or create topic
        const char *type_name = type_support_gpsinfo->get_type_name();
        const char *topic_str = "MAVLink__GPSInfo";
        DDS::Duration_t timeout = {0, 0};
        DDS::Topic_var topic = participant->find_topic(topic_str, timeout);

        DDS::ReturnCode_t status;
        DDS::TopicQos topicQos;
        status = participant->get_default_topic_qos(topicQos);
        topicQos.durability.kind = DDS::VOLATILE_DURABILITY_QOS;
        topicQos.reliability.kind = DDS::RELIABLE_RELIABILITY_QOS;
        const int RELIABILITY_TIMEOUT = 20;
        topicQos.reliability.max_blocking_time.sec = RELIABILITY_TIMEOUT;
        topicQos.history.kind = DDS::KEEP_LAST_HISTORY_QOS;
        topicQos.history.depth = 1;
        if (topic.in() == NULL) {
            topic = participant->create_topic(topic_str, type_name, topicQos,
                                              0, DDS::STATUS_MASK_NONE);
            if (topic.in() == NULL) {
                DEBUG_MSG(CFE_SB_DDS_ERR_EID, CFE_EVS_ERROR, "Topic creation failed.");
                throw std::exception();
            }
        }

        // Create data writer for topic if necessary
        DDS::DataWriter_var writer = pub->lookup_datawriter(topic->get_name());
        if (writer.in() == NULL) {
            writer = pub->create_datawriter(topic, DATAWRITER_QOS_DEFAULT, NULL,
                                            DDS::STATUS_MASK_NONE);
            if (writer.in() == NULL) {
                DEBUG_MSG(CFE_SB_DDS_ERR_EID, CFE_EVS_ERROR,
                          "Data writer creation failed.");
                throw std::exception();
            }
        }

        writer_gpsinfo = GPSInfoDataWriter::_narrow(writer);
        if (writer_gpsinfo.in() == NULL) {
            DEBUG_MSG(CFE_SB_DDS_ERR_EID, CFE_EVS_ERROR,
                      "Message data writer creation failed.");
            throw std::exception();
        }

    } catch (std::exception &e) {
        DEBUG_MSG(CFE_SB_DDS_ERR_EID, CFE_EVS_ERROR, "Exception caught: %s",
                  e.what());
        return 6;
    }
    return 0;
}


void GPSInfo_Send(
        unsigned long id,
        double time_gps,
        double latitude,
        double longitude,
        double altitude_abs,
        double altitude_rel,
        double vx,
        double vy,
        double vz) {

    try {
        GPSInfo msg;
        msg.id = id;
        msg.time = time_gps;
        msg.latitude = latitude;
        msg.longitude = longitude;
        msg.absoluteAltitude = altitude_abs;
        msg.relativeAltitude = altitude_rel;
        msg.xVelocity = vx;
        msg.yVelocity = vy;
        msg.zVelocity = vz;

        DDS::ReturnCode_t error = writer_gpsinfo->write(msg, DDS::HANDLE_NIL);
        if (DDS::RETCODE_OK != error) {
            DEBUG_MSG(CFE_SB_DDS_ERR_EID, CFE_EVS_ERROR, "Write failed.");
            throw std::exception();
        }
    } catch (std::exception &e) {
        DEBUG_MSG(CFE_SB_DDS_ERR_EID, CFE_EVS_ERROR, "Exception caught: %s",
                  e.what());
        throw e;
    }
}

#endif
