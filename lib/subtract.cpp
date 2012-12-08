// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#include <grextras/subtract.hpp>
#include <boost/make_shared.hpp>
#include <stdexcept>
#include <complex>
#include <volk/volk.h>

using namespace grextras;

/***********************************************************************
 * Templated Subtractor class
 **********************************************************************/
template <typename type>
struct SubtractImpl : Subtract
{
    SubtractImpl(const size_t vlen):
        gras::Block("GrExtras Subtract"),
        _vlen(vlen)
    {
        this->set_input_signature(gras::IOSignature(sizeof(type)*_vlen));
        this->set_output_signature(gras::IOSignature(sizeof(type)*_vlen));
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
 * Generic Subtractor implementation
 **********************************************************************/
template <typename type>
void SubtractImpl<type>::work(
    const InputItems &ins, const OutputItems &outs
){
    const size_t n_nums = std::min(ins.min(), outs.min());
    type *out = outs[0].cast<type *>();
    const type *in0 = ins[0].cast<const type *>();

    if (ins.size() == 1)
    {
        for (size_t i = 0; i < n_nums * _vlen; i++)
        {
            out[i] = - in0[i];
        }
    }

    else for (size_t n = 1; n < ins.size(); n++)
    {
        const type *in = ins[n].cast<const type *>();
        for (size_t i = 0; i < n_nums * _vlen; i++)
        {
            out[i] = in0[i] - in[i];
        }
        in0 = out; //for next input, we do output -= input
    }

    this->consume(n_nums);
    this->produce(n_nums);
}

/***********************************************************************
 * factory function
 **********************************************************************/
Subtract::sptr Subtract::make_fc32_fc32(const size_t vlen)
{
    return sptr(new SubtractImpl<std::complex<float> >(vlen));
}

Subtract::sptr Subtract::make_sc32_sc32(const size_t vlen)
{
    return sptr(new SubtractImpl<std::complex<boost::int32_t> >(vlen));
}

Subtract::sptr Subtract::make_sc16_sc16(const size_t vlen)
{
    return sptr(new SubtractImpl<std::complex<boost::int16_t> >(vlen));
}

Subtract::sptr Subtract::make_sc8_sc8(const size_t vlen)
{
    return sptr(new SubtractImpl<std::complex<boost::int8_t> >(vlen));
}

Subtract::sptr Subtract::make_f32_f32(const size_t vlen)
{
    return sptr(new SubtractImpl<float>(vlen));
}

Subtract::sptr Subtract::make_s32_s32(const size_t vlen)
{
    return sptr(new SubtractImpl<boost::int32_t>(vlen));
}

Subtract::sptr Subtract::make_s16_s16(const size_t vlen)
{
    return sptr(new SubtractImpl<boost::int16_t>(vlen));
}

Subtract::sptr Subtract::make_s8_s8(const size_t vlen)
{
    return sptr(new SubtractImpl<boost::int8_t>(vlen));
}

