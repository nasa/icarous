#ifndef DDS_COMMON_HPP
#define DDS_COMMON_HPP

#include <future>

#include <dds/dds.hpp>

#include "DDS/TopicNameTraits.hpp"
#include "Utils/Types.hpp"

extern const UInt32 DDS_ICAROUS_DOMAIN_ID;
extern const dds::core::Duration RELIABILITY_TIMEOUT;

dds::topic::qos::TopicQos defaultTopicQos(dds::domain::DomainParticipant &dp);

template<class Topic>
dds::pub::DataWriter<Topic> DataWriterFactory() {
    dds::domain::DomainParticipant dp(DDS_ICAROUS_DOMAIN_ID);
    dds::topic::qos::TopicQos topicQos = defaultTopicQos(dp);
    dds::topic::Topic<Topic> topic(dp, TopicNameTraits<Topic>::name, topicQos);
    dds::pub::qos::PublisherQos pubQos = dp.default_publisher_qos();
    dds::pub::Publisher pub(dp, pubQos);
    dds::pub::qos::DataWriterQos dwqos = topic.qos();
    dds::pub::DataWriter<Topic> dw(pub, topic, dwqos);
    return dw;
}


#endif
