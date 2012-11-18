// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#include <grextras/subtract.hpp>
#include <boost/make_shared.hpp>
#include <boost/cstdint.hpp>
#include <stdexcept>
#include <complex>
#include <volk/volk.h>

using namespace grextras;

/***********************************************************************
 * Templated Subtractor class
 **********************************************************************/
template <typename type>
class SubtractImpl : public Subtract{
public:
    SubtractImpl(const size_t num_inputs, const size_t vlen):
        SyncBlock("GrExtras Subtract"),
        _vlen(vlen)
    {
        this->set_input_signature(gras::IOSignature(sizeof(type)));
        this->set_output_signature(gras::IOSignature(sizeof(type)));
    }

    size_t sync_work(
        const InputItems &input_items,
        const OutputItems &output_items
    );

private:
    const size_t _vlen;
};

/***********************************************************************
 * Generic Subtractor implementation
 **********************************************************************/
template <typename type>
size_t SubtractImpl<type>::sync_work(
    const InputItems &input_items,
    const OutputItems &output_items
){
    const size_t n_nums = output_items[0].size() * _vlen;
    type *out = output_items[0].cast<type *>();
    const type *in0 = input_items[0].cast<const type *>();

    if (input_items.size() == 1)
    {
        for (size_t i = 0; i < n_nums; i++)
        {
            out[i] = - in0[i];
        }
    }

    else for (size_t n = 1; n < input_items.size(); n++)
    {
        const type *in = input_items[n].cast<const type *>();
        for (size_t i = 0; i < n_nums; i++)
        {
            out[i] = in0[i] - in[i];
        }
        in0 = out; //for next input, we do output -= input
    }

    return output_items[0].size();
}

/***********************************************************************
 * factory function
 **********************************************************************/
Subtract::sptr Subtract::make_fc32_fc32(const size_t num_inputs, const size_t vlen)
{
    return sptr(new SubtractImpl<std::complex<float> >(num_inputs, vlen));
}

Subtract::sptr Subtract::make_sc32_sc32(const size_t num_inputs, const size_t vlen)
{
    return sptr(new SubtractImpl<std::complex<boost::int32_t> >(num_inputs, vlen));
}

Subtract::sptr Subtract::make_sc16_sc16(const size_t num_inputs, const size_t vlen)
{
    return sptr(new SubtractImpl<std::complex<boost::int16_t> >(num_inputs, vlen));
}

Subtract::sptr Subtract::make_sc8_sc8(const size_t num_inputs, const size_t vlen)
{
    return sptr(new SubtractImpl<std::complex<boost::int8_t> >(num_inputs, vlen));
}

Subtract::sptr Subtract::make_f32_f32(const size_t num_inputs, const size_t vlen)
{
    return sptr(new SubtractImpl<float>(num_inputs, vlen));
}

Subtract::sptr Subtract::make_s32_s32(const size_t num_inputs, const size_t vlen)
{
    return sptr(new SubtractImpl<boost::int32_t>(num_inputs, vlen));
}

Subtract::sptr Subtract::make_s16_s16(const size_t num_inputs, const size_t vlen)
{
    return sptr(new SubtractImpl<boost::int16_t>(num_inputs, vlen));
}

Subtract::sptr Subtract::make_s8_s8(const size_t num_inputs, const size_t vlen)
{
    return sptr(new SubtractImpl<boost::int8_t>(num_inputs, vlen));
}

