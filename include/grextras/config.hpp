// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#ifndef INCLUDED_GREXTRAS_CONFIG_HPP
#define INCLUDED_GREXTRAS_CONFIG_HPP

#include <ciso646>

// http://gcc.gnu.org/wiki/Visibility
// Generic helper definitions for shared library support
#if defined _WIN32 || defined __CYGWIN__
  #define GREXTRAS_HELPER_DLL_IMPORT __declspec(dllimport)
  #define GREXTRAS_HELPER_DLL_EXPORT __declspec(dllexport)
  #define GREXTRAS_HELPER_DLL_LOCAL
#else
  #if __GNUC__ >= 4
    #define GREXTRAS_HELPER_DLL_IMPORT __attribute__ ((visibility ("default")))
    #define GREXTRAS_HELPER_DLL_EXPORT __attribute__ ((visibility ("default")))
    #define GREXTRAS_HELPER_DLL_LOCAL  __attribute__ ((visibility ("hidden")))
  #else
    #define GREXTRAS_HELPER_DLL_IMPORT
    #define GREXTRAS_HELPER_DLL_EXPORT
    #define GREXTRAS_HELPER_DLL_LOCAL
  #endif
#endif

#define GREXTRAS_DLL //always build a dll

// Now we use the generic helper definitions above to define GREXTRAS_API and GREXTRAS_LOCAL.
// GREXTRAS_API is used for the public API symbols. It either DLL imports or DLL exports (or does nothing for static build)
// GREXTRAS_LOCAL is used for non-api symbols.

#ifdef GREXTRAS_DLL // defined if GRAS is compiled as a DLL
  #ifdef GREXTRAS_DLL_EXPORTS // defined if we are building the GRAS DLL (instead of using it)
    #define GREXTRAS_API GREXTRAS_HELPER_DLL_EXPORT
  #else
    #define GREXTRAS_API GREXTRAS_HELPER_DLL_IMPORT
  #endif // GREXTRAS_DLL_EXPORTS
  #define GREXTRAS_LOCAL GREXTRAS_HELPER_DLL_LOCAL
#else // GREXTRAS_DLL is not defined: this means GRAS is a static lib.
  #define GREXTRAS_API
  #define GREXTRAS_LOCAL
#endif // GREXTRAS_DLL

#include <complex>
#include <boost/cstdint.hpp>

namespace grextras
{

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

}

#endif /*INCLUDED_GREXTRAS_CONFIG_HPP*/
