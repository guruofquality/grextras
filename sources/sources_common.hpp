// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#ifndef INCLUDED_GREXTRAS_LIB_SOURCES_COMMON_HPP
#define INCLUDED_GREXTRAS_LIB_SOURCES_COMMON_HPP

#include <complex>

typedef std::complex<double> complex128;
typedef std::complex<float> complex64;

template <typename type>
complex128 num_to_complex128(const std::complex<type> &val)
{
    return complex128(double(val.real()), double(val.imag()));
}

template <typename type>
complex128 num_to_complex128(const type &val)
{
    return complex128(double(val));
}

template <typename type>
void complex128_to_num(const complex128 &in, std::complex<type> &out)
{
    out = std::complex<type>(in);
}

template <typename type>
void complex128_to_num(const complex128 &in, type &out)
{
    out = type(in.real());
}

#endif //INCLUDED_GREXTRAS_LIB_SOURCES_COMMON_HPP
