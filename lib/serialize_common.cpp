// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#include <PMC/Serialize.hpp>
#include <gras/sbuffer.hpp>
#include <boost/serialization/split_free.hpp>
#include <boost/serialization/string.hpp>

namespace boost { namespace serialization {
template<class Archive>
void save(Archive & ar, const gras::SBuffer & b, unsigned int version)
{
    //TODO lazyness string
    std::string s((const char *)b.get(), b.length);
    ar & s;
}
template<class Archive>
void load(Archive & ar, gras::SBuffer & b, unsigned int version)
{
    //TODO lazyness string
    std::string s;
    ar & s;
    gras::SBufferConfig config;
    config.length = s.length();
    b = gras::SBuffer(config);
    std::memcpy(b.get(), s.c_str(), s.length());
}
}}

BOOST_SERIALIZATION_SPLIT_FREE(gras::SBuffer)
PMC_SERIALIZE_EXPORT(gras::SBuffer, "PMC<gras::SBuffer>")
