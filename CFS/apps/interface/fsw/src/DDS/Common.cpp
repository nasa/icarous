#include "DDS/Common.hpp"


const u_int32_t DDS_ICAROUS_DOMAIN_ID = org::opensplice::domain::default_id();

const dds::core::Duration RELIABILITY_TIMEOUT = dds::core::Duration::from_secs(20);

dds::topic::qos::TopicQos defaultTopicQos(dds::domain::DomainParticipant &dp) {
    return dp.default_topic_qos()
            << dds::core::policy::Durability::Volatile()
            << dds::core::policy::History::KeepLast(1)
            << dds::core::policy::Reliability::Reliable(RELIABILITY_TIMEOUT);
}

