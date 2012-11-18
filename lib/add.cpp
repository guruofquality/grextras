// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#include <grextras/add.hpp>
#include <boost/make_shared.hpp>
#include <boost/cstdint.hpp>
#include <stdexcept>
#include <complex>
#include <volk/volk.h>

using namespace grextras;

/***********************************************************************
 * Templated Adder class
 **********************************************************************/
template <typename type>
class AddImpl : public Add{
public:
    AddImpl(const size_t num_inputs, const size_t vlen):
        SyncBlock("GrExtras Add"),
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
 * Generic Adder implementation
 **********************************************************************/
template <typename type>
size_t AddImpl<type>::sync_work(
    const InputItems &input_items,
    const OutputItems &output_items
){

    const size_t n_nums = output_items[0].size();

    type *out = output_items[0].cast<type *>();
    const type *in0 = input_items[0].cast<const type *>();

    for (size_t n = 1; n < input_items.size(); n++)
    {
        const type *in = input_items[n].cast<const type *>();
        for (size_t i = 0; i < n_nums * _vlen; i++)
        {
            out[i] = in0[i] + in[i];
        }
        in0 = out; //for next input, we do output += input
    }

    return n_nums;
}

/***********************************************************************
 * Adder implementation with float32 - calls volk
 **********************************************************************/
template <>
size_t AddImpl<float>::sync_work(
    const InputItems &input_items,
    const OutputItems &output_items
){
    const size_t n_nums = output_items[0].size();

    float *out = output_items[0].cast<float *>();
    const float *in0 = input_items[0].cast<const float *>();

    for (size_t n = 1; n < input_items.size(); n++)
    {
        const float *in = input_items[n].cast<const float *>();
        volk_32f_x2_add_32f(out, in0, in, n_nums * _vlen);
        in0 = out; //for next input, we do output += input
    }

    return n_nums;
}

/***********************************************************************
 * factory function
 **********************************************************************/
Add::sptr Add::make_fc32_fc32(const size_t num_inputs, const size_t vlen)
{
    return sptr(new AddImpl<float>(num_inputs, 2*vlen));
}

Add::sptr Add::make_sc32_sc32(const size_t num_inputs, const size_t vlen)
{
    return sptr(new AddImpl<boost::int32_t>(num_inputs, 2*vlen));
}

Add::sptr Add::make_sc16_sc16(const size_t num_inputs, const size_t vlen)
{
    return sptr(new AddImpl<boost::int16_t>(num_inputs, 2*vlen));
}

Add::sptr Add::make_sc8_sc8(const size_t num_inputs, const size_t vlen)
{
    return sptr(new AddImpl<boost::int8_t>(num_inputs, 2*vlen));
}

Add::sptr Add::make_f32_f32(const size_t num_inputs, const size_t vlen)
{
    return sptr(new AddImpl<float>(num_inputs, vlen));
}

Add::sptr Add::make_s32_s32(const size_t num_inputs, const size_t vlen)
{
    return sptr(new AddImpl<boost::int32_t>(num_inputs, vlen));
}

Add::sptr Add::make_s16_s16(const size_t num_inputs, const size_t vlen)
{
    return sptr(new AddImpl<boost::int16_t>(num_inputs, vlen));
}

Add::sptr Add::make_s8_s8(const size_t num_inputs, const size_t vlen)
{
    return sptr(new AddImpl<boost::int8_t>(num_inputs, vlen));
}

