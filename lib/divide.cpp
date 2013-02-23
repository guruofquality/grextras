// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#include <grextras/divide.hpp>
#include <boost/make_shared.hpp>
#include <stdexcept>
#include <complex>
#include <volk/volk.h>

using namespace grextras;

/***********************************************************************
 * Templated Divider class
 **********************************************************************/
template <typename type>
struct DivideImpl : Divide
{
    DivideImpl(const size_t vlen):
        gras::Block("GrExtras Divide"),
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
 * Generic Divider implementation
 **********************************************************************/
template <typename type>
void DivideImpl<type>::work(
    const InputItems &ins, const OutputItems &outs
){
    const size_t n_nums = std::min(ins.min(), outs.min());
    type *out = outs[0].cast<type *>();
    const type *in0 = ins[0].cast<const type *>();

    if (ins.size() == 1)
    {
        for (size_t i = 0; i < n_nums * _vlen; i++)
        {
            out[i] = type(1) / in0[i];
        }
    }

    else for (size_t n = 1; n < ins.size(); n++)
    {
        const type *in = ins[n].cast<const type *>();
        for (size_t i = 0; i < n_nums * _vlen; i++)
        {
            out[i] = in0[i] / in[i];
        }
        in0 = out; //for next input, we do output /= input
    }

    this->consume(n_nums);
    this->produce(n_nums);
}

/***********************************************************************
 * factory function
 **********************************************************************/
Divide::sptr Divide::make_fc32_fc32(const size_t vlen)
{
    return sptr(new DivideImpl<std::complex<float> >(vlen));
}

Divide::sptr Divide::make_sc32_sc32(const size_t vlen)
{
    return sptr(new DivideImpl<std::complex<boost::int32_t> >(vlen));
}

Divide::sptr Divide::make_sc16_sc16(const size_t vlen)
{
    return sptr(new DivideImpl<std::complex<boost::int16_t> >(vlen));
}

Divide::sptr Divide::make_sc8_sc8(const size_t vlen)
{
    return sptr(new DivideImpl<std::complex<boost::int8_t> >(vlen));
}

Divide::sptr Divide::make_f32_f32(const size_t vlen)
{
    return sptr(new DivideImpl<float>(vlen));
}

Divide::sptr Divide::make_s32_s32(const size_t vlen)
{
    return sptr(new DivideImpl<boost::int32_t>(vlen));
}

Divide::sptr Divide::make_s16_s16(const size_t vlen)
{
    return sptr(new DivideImpl<boost::int16_t>(vlen));
}

Divide::sptr Divide::make_s8_s8(const size_t vlen)
{
    return sptr(new DivideImpl<boost::int8_t>(vlen));
}

