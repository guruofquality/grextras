// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#include <grextras/add.hpp>
#include <boost/make_shared.hpp>
#include <stdexcept>
#include <complex>
#ifdef HAVE_VOLK
#include <volk/volk.h>
#endif

using namespace grextras;

/***********************************************************************
 * Templated Adder class
 **********************************************************************/
template <typename type>
struct AddImpl : Add
{
    AddImpl(const size_t vlen):
        gras::Block("GrExtras Add"),
        _vlen(vlen)
    {
        this->set_input_size(0, sizeof(type)*_vlen);
        this->set_output_size(0, sizeof(type)*_vlen);
    }

    void notify_topology(const size_t num_inputs, const size_t num_outputs)
    {
        for (size_t i = 0; i < num_inputs; i++)
        {
            gras::InputPortConfig config = this->get_input_config(i);
            config.inline_buffer = (i == 0);
            this->set_input_config(i, config);
        }
    }

    void work(const InputItems &, const OutputItems &);

    const size_t _vlen;
};

/***********************************************************************
 * Generic Adder implementation
 **********************************************************************/
template <typename type>
void AddImpl<type>::work(
    const InputItems &ins, const OutputItems &outs
){
    const size_t n_nums = std::min(ins.min(), outs.min());
    type *out = outs[0].cast<type *>();
    const type *in0 = ins[0].cast<const type *>();

    for (size_t n = 1; n < ins.size(); n++)
    {
        const type *in = ins[n].cast<const type *>();
        for (size_t i = 0; i < n_nums * _vlen; i++)
        {
            out[i] = in0[i] + in[i];
        }
        in0 = out; //for next input, we do output += input
    }

    this->consume(n_nums);
    this->produce(n_nums);
}

#ifdef HAVE_VOLK
/***********************************************************************
 * Adder implementation with float32 - calls volk
 **********************************************************************/
template <>
void AddImpl<float>::work(
    const InputItems &ins, const OutputItems &outs
){
    const size_t n_nums = std::min(ins.min(), outs.min());
    float *out = outs[0].cast<float *>();
    const float *in0 = ins[0].cast<const float *>();

    for (size_t n = 1; n < ins.size(); n++)
    {
        const float *in = ins[n].cast<const float *>();
        volk_32f_x2_add_32f(out, in0, in, n_nums * _vlen);
        in0 = out; //for next input, we do output += input
    }

    this->consume(n_nums);
    this->produce(n_nums);
}
#endif

/***********************************************************************
 * factory function
 **********************************************************************/
Add::sptr Add::make_fc32_fc32(const size_t vlen)
{
    return sptr(new AddImpl<float>(2*vlen));
}

Add::sptr Add::make_sc32_sc32(const size_t vlen)
{
    return sptr(new AddImpl<boost::int32_t>(2*vlen));
}

Add::sptr Add::make_sc16_sc16(const size_t vlen)
{
    return sptr(new AddImpl<boost::int16_t>(2*vlen));
}

Add::sptr Add::make_sc8_sc8(const size_t vlen)
{
    return sptr(new AddImpl<boost::int8_t>(2*vlen));
}

Add::sptr Add::make_f32_f32(const size_t vlen)
{
    return sptr(new AddImpl<float>(vlen));
}

Add::sptr Add::make_s32_s32(const size_t vlen)
{
    return sptr(new AddImpl<boost::int32_t>(vlen));
}

Add::sptr Add::make_s16_s16(const size_t vlen)
{
    return sptr(new AddImpl<boost::int16_t>(vlen));
}

Add::sptr Add::make_s8_s8(const size_t vlen)
{
    return sptr(new AddImpl<boost::int8_t>(vlen));
}

