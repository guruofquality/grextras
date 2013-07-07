// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#ifndef INCLUDED_GREXTRAS_MULTIPLY_CONST_HPP
#define INCLUDED_GREXTRAS_MULTIPLY_CONST_HPP

#include <grextras/config.hpp>
#include <gras/hier_block.hpp>
#include <gras/block.hpp>

namespace grextras
{

/*!
 * Multiply input with the specified number.
 * The number can be configured with set_const(new_val).
 */
struct GREXTRAS_API MultiplyConst : virtual gras::HierBlock
{
    typedef boost::shared_ptr<MultiplyConst> sptr;

    static sptr make_fc32_fc32(const std::complex<double> &val = 0);
    static sptr make_sc32_sc32(const std::complex<double> &val = 0);
    static sptr make_sc16_sc16(const std::complex<double> &val = 0);
    static sptr make_sc8_sc8(const std::complex<double> &val = 0);
    static sptr make_f32_f32(const std::complex<double> &val = 0);
    static sptr make_s32_s32(const std::complex<double> &val = 0);
    static sptr make_s16_s16(const std::complex<double> &val = 0);
    static sptr make_s8_s8(const std::complex<double> &val = 0);
};

/*!
 * Multiply input with the specified vector.
 * The vector can be configured with set_const(new_val).
 */
class GREXTRAS_API MultiplyConstV : virtual public gras::Block
{
public:
    typedef boost::shared_ptr<MultiplyConstV> sptr;

    /*!
     * Make a multiply const from type and const value.
     * The length of value is the vector length of this block.
     */
    static sptr make_fc32_fc32(const std::vector<std::complex<float> > &vec);
    static sptr make_sc32_sc32(const std::vector<std::complex<boost::int32_t> > &vec);
    static sptr make_sc16_sc16(const std::vector<std::complex<boost::int16_t> > &vec);
    static sptr make_sc8_sc8(const std::vector<std::complex<boost::int8_t> > &vec);
    static sptr make_f32_f32(const std::vector<float> &vec);
    static sptr make_s32_s32(const std::vector<boost::int32_t> &vec);
    static sptr make_s16_s16(const std::vector<boost::int16_t> &vec);
    static sptr make_s8_s8(const std::vector<boost::int8_t> &vec);
};

}

#endif /*INCLUDED_GREXTRAS_MULTIPLY_CONST_HPP*/
