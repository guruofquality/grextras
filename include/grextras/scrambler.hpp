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
 * The following properties are available through get and set:
 *  - polynomial <uint64> - LFSR polynomial taps
 *    - Polynomial representation: x^4 + x^3 + 1 = 11001 = 0x19
 *  - seed <uint64> - LFSR initialization value
 *  - mode <string> - "additive" or "multiplicative"
 *  - sync_word <string> - pattern of 1s and 0s
 *    - Example pattern: "100111010"
 *  - frame_size <long> - frame length in bits
 *
 * The sync word and frame size are only used in additive mode.
 * Every frame_size bits, the sync word is sent,
 * and the LFSR is reset with the seed value.
 */
struct Scrambler
{
    //! Scrambler factory function
    GREXTRAS_API static gras::Block *make(void);
};

}

#endif /*INCLUDED_GREXTRAS_SCRAMBLER_HPP*/
