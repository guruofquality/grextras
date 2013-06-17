// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

%include "GrExtras_Common.i"

%{
#include <grextras/add.hpp>
#include <grextras/add_const.hpp>
#include <grextras/subtract.hpp>
#include <grextras/multiply.hpp>
#include <grextras/multiply_const.hpp>
#include <grextras/divide.hpp>
%}

%include <stdint.i>
%include <std_vector.i>
%include <std_complex.i>

%apply short { boost::uint8_t };
%apply char { boost::int8_t };
%apply int { boost::uint16_t };
%apply short { boost::int16_t };
%apply int { boost::int32_t };
%apply long long { boost::uint32_t };
%apply long long { boost::int64_t };
%apply long long { boost::uint64_t };

%template(grextras_std_vec_complex_double) std::vector<std::complex<double> >;
%template(grextras_std_vec_complex_float) std::vector<std::complex<float> >;
%template(grextras_std_vec_complex_int32) std::vector<std::complex<int> >;
%template(grextras_std_vec_complex_int16) std::vector<std::complex<short> >;
%template(grextras_std_vec_complex_int8) std::vector<std::complex<char> >;

%template(grextras_std_vec_double) std::vector<double>;
%template(grextras_std_vec_float) std::vector<float>;
%template(grextras_std_vec_int32) std::vector<int>;
%template(grextras_std_vec_int16) std::vector<short>;
%template(grextras_std_vec_int8) std::vector<char>;

%include <grextras/add.hpp>
%include <grextras/add_const.hpp>
%include <grextras/subtract.hpp>
%include <grextras/multiply.hpp>
%include <grextras/multiply_const.hpp>
%include <grextras/divide.hpp>

%template(set_const) grextras::MultiplyConstV::set_const<std::complex<double> >;
%template(set_const) grextras::AddConstV::set_const<std::complex<double> >;

GREXTRAS_SWIG_FOO(Add)
GREXTRAS_SWIG_FOO(AddConst)
GREXTRAS_SWIG_FOO(AddConstV)
GREXTRAS_SWIG_FOO(Subtract)
GREXTRAS_SWIG_FOO(Multiply)
GREXTRAS_SWIG_FOO(MultiplyConst)
GREXTRAS_SWIG_FOO(MultiplyConstV)
GREXTRAS_SWIG_FOO(Divide)
