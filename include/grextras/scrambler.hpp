// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#ifndef INCLUDED_GREXTRAS_SCRAMBLER_HPP
#define INCLUDED_GREXTRAS_SCRAMBLER_HPP

#include <grextras/config.hpp>
#include <gras/block.hpp>

namespace grextras
{

/*!
 * The scrambler block.
 * This block performs additive or multiplicative scrambling
 * with programmable LFSR taps, seed value, and sync word.
 * 
 * The following calls are available through the callable interface:
 *  - set_poly <int64> - LFSR polynomial taps
 *    - Polynomial representation: x^4 + x^3 + 1 = 11001 = 0x19
 *  - set_seed <int64> - LFSR initialization value
 *  - set_mode <string> - "additive" or "multiplicative"
 *  - set_sync <string> - pattern of 1s and 0s
 *    - Example pattern: "100111010"
 *
 * When the length tag is detected,
 * the sync word is injected into the stream,
 * and the LFSR is reset to the seed value.
 */
struct Scrambler
{
    //! Scrambler factory function
    GREXTRAS_API static gras::Block *make(void);
};

}

#endif /*INCLUDED_GREXTRAS_SCRAMBLER_HPP*/
