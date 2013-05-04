// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#ifndef INCLUDED_GREXTRAS_SERIALIZE_COMMON_HPP
#define INCLUDED_GREXTRAS_SERIALIZE_COMMON_HPP

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
    config.length = s.length;
    b = gras::SBuffer(config);
    std::memcpy(b.get(), s.c_str(), s.length());
}
}}

PMC_SERIALIZE_EXPORT(gras::SBuffer, "PMC<gras::SBuffer>")

#include <PMC/SerializeTypes.hpp>
#include <gras/tags.hpp>
#include <boost/cstdint.hpp>

const boost::uint32_t VRLP = 0
    | (boost::uint32_t('V') << 24)
    | (boost::uint32_t('R') << 16)
    | (boost::uint32_t('L') << 8)
    | (boost::uint32_t('P') << 0)
;

const boost::uint32_t VEND = 0
    | (boost::uint32_t('V') << 24)
    | (boost::uint32_t('E') << 16)
    | (boost::uint32_t('N') << 8)
    | (boost::uint32_t('D') << 0)
;

//needed to fit headers and footers
const size_t PAD_BYTES = 8;

const int VITA_SID = (1 << 28);
const int VITA_EXT = (1 << 29);
const int VITA_TSF = (1 << 20);

#endif //INCLUDED_GREXTRAS_SERIALIZE_COMMON_HPP
