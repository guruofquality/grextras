// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#ifndef INCLUDED_GREXTRAS_SERIALIZE_COMMON_HPP
#define INCLUDED_GREXTRAS_SERIALIZE_COMMON_HPP

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
