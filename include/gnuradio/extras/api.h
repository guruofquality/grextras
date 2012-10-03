/*
 * Copyright 2011-2012 Free Software Foundation, Inc.
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

#ifndef INCLUDED_GR_EXTRAS_API_H
#define INCLUDED_GR_EXTRAS_API_H

#include <ciso646>

#include <gruel/attributes.h>

#ifdef gnuradio_extras_EXPORTS
#  define GR_EXTRAS_API __GR_ATTR_EXPORT
#else
#  define GR_EXTRAS_API __GR_ATTR_IMPORT
#endif

#include <complex>

template <typename type>
std::complex<double> gr_num_to_complex_double(const std::complex<type> &val){
    return std::complex<double>(double(val.real()), double(val.imag()));
}

template <typename type>
std::complex<double> gr_num_to_complex_double(const type &val){
    return std::complex<double>(double(val));
}

template <typename type>
void gr_complex_double_to_num(const std::complex<double> &in, std::complex<type> &out){
    out = std::complex<type>(in);
}

template <typename type>
void gr_complex_double_to_num(const std::complex<double> &in, type &out){
    out = type(in.real());
}

#endif /* INCLUDED_GR_EXTRAS_API_H */
