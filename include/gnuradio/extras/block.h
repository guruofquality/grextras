/*
 * Copyright 2012 Free Software Foundation, Inc.
 * 
 * This file is part of GNU Radio
 * 
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 * 
 * GNU Radio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */
#ifndef INCLUDED_GR_EXTRAS_BLOCK_H
#define INCLUDED_GR_EXTRAS_BLOCK_H

#include <gnuradio/extras/api.h>
#include <gr_hier_block2.h>

namespace gnuradio{ namespace extras{

/*!
 * The base clock class that provides message passing,
 * and a more object oriented access to work buffers.
 * This block implements basic and sync block functionalities.
 */
class GR_EXTRAS_API block : virtual public gr_hier_block2{
public:

    /*!
     * The block constructor.
     * The message output ports creates optional output ports,
     * considered to be indexed after the output IO signature.
     * Additionaly, there is 1 optional input message port,
     * considered to be indexed after the input IO signature.
     * \param name the name of this block
     * \param in_sig the input signature
     * \param out_sig the output signature
     * \param num_msg_outs number message output ports
     */
    block(
        const std::string &name,
        gr_io_signature_sptr in_sig,
        gr_io_signature_sptr out_sig,
        const size_t num_msg_outs = 1
    );

    //! deconstructor
    ~block(void);

    //! set the decimation rate for decimating sync blocks
    void set_decim(const size_t decim);

    //! get the decimation as set by the user (default 1)
    size_t get_decim(void);

    //! set the interpolation rate for interpolating sync blocks
    void set_interp(const size_t interp);

    //! get the interpolation as set by the user (default 1)
    size_t get_interp(void);

private:
    //forward declared private guts
    struct impl;
    boost::shared_ptr<impl> _impl;
};

}}

#endif /* INCLUDED_GR_EXTRAS_BLOCK_H */
