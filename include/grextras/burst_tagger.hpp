// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#ifndef INCLUDED_GREXTRAS_BURST_TAGGER_HPP
#define INCLUDED_GREXTRAS_BURST_TAGGER_HPP

#include <grextras/config.hpp>
#include <gras/block.hpp>

namespace grextras
{

/*!
 * The burst tagger interpolates an end of burst tag
 * onto a stream from a discontinous stream of samples.
 *
 * A length tag embedded into the input sample stream
 * tells the burst tagger block where to place the EOB tag.
 *
 * This block is meant to be used after a modulator block:
 * packet_framer -> modulator -> burst_tagger -> USRP Sink
 */
struct GREXTRAS_API BurstTagger : virtual gras::Block
{
    typedef boost::shared_ptr<BurstTagger> sptr;

    /*!
     * Make a new burst tagger block.
     * \param sps the modulator's samples per symbol
     */
    static sptr make(const size_t sps);
};

}


#endif /*INCLUDED_GREXTRAS_BURST_TAGGER_HPP*/
