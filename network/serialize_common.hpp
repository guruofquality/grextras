// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#ifndef INCLUDED_GREXTRAS_SERIALIZE_COMMON_HPP
#define INCLUDED_GREXTRAS_SERIALIZE_COMMON_HPP

#include <gras/tags.hpp>
#include <boost/cstdint.hpp>

static const boost::uint32_t VRLP = 0
    | (boost::uint32_t('V') << 24)
    | (boost::uint32_t('R') << 16)
    | (boost::uint32_t('L') << 8)
    | (boost::uint32_t('P') << 0)
;

static const boost::uint32_t VEND = 0
    | (boost::uint32_t('V') << 24)
    | (boost::uint32_t('E') << 16)
    | (boost::uint32_t('N') << 8)
    | (boost::uint32_t('D') << 0)
;

static const int VITA_SID = (1 << 28);
static const int VITA_EXT = (1 << 29);
static const int VITA_TSF = (1 << 20);

//minimum packet size given headers + footers
static const size_t MIN_PKT_BYTES = 20;

//needed to fit headers and footers
static const size_t HDR_TLR_BYTES = 8*4;

//we need a practical limit because VRL packets can be 3 MiB
static const size_t MAX_PKT_BYTES = 128*1024;

#include <iostream>

#define ASSERT(x) {if GRAS_UNLIKELY(not (x)) \
{ \
    std::cerr << "ASSERT FAIL " << __FILE__ << ":" << __LINE__ << "\n\t" << #x << std::endl << std::flush; \
    throw std::runtime_error(std::string("ASSERT FAIL ") + #x); \
}}

#endif //INCLUDED_GREXTRAS_SERIALIZE_COMMON_HPP
