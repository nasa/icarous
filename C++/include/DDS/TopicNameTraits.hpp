#ifndef DDS_TOPICNAMETRAITS_HPP
#define DDS_TOPICNAMETRAITS_HPP

#include <string>


template<typename T>
struct TopicNameTraits {
    static std::string const name;
};

#define REGISTER_TOPIC_NAME(X,Y) template <> const std::string TopicNameTraits<X>::name = Y


#endif
