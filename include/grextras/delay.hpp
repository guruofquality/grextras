// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#ifndef INCLUDED_GREXTRAS_DELAY_HPP
#define INCLUDED_GREXTRAS_DELAY_HPP

#include <grextras/config.hpp>
#include <gras/block.hpp>

namespace grextras
{

/*!
 * The delay block imposes a delay in a stream of items.
 */
struct GREXTRAS_API Delay : virtual gras::Block
{
    typedef boost::shared_ptr<Delay> sptr;

    /*!
     * Make a new delay block.
     * \param itemsize the size of an item in bytes
     */
    sptr make(const size_t itemsize);

    /*!
     * The delay is measured in an exact count of items.
     * If the delay is positive, zero-valued items will be inserted into the stream.
     * If the delay is negative, items will be remove from the stream.
     * If the delay value is set before the flow graph is executed,
     * the delay will be imposed at the begining of the stream.
     * However, if the flow graph is already running,
     * the delay injection will be non-deterministic.
     * \param num_items a count of items to impose delay
     */
    virtual void set_delay(const int num_items) = 0;
};

}


#endif /*INCLUDED_GREXTRAS_DELAY_HPP*/
