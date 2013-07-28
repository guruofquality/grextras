// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#include <gras/block.hpp>
#include <gras/factory.hpp>
#include <stdexcept>
#include <complex>
#include <volk/volk.h>

/***********************************************************************
 * Templated Divider class
 **********************************************************************/
template <typename type>
struct Divide : gras::Block
{
    Divide(const size_t vlen):
        gras::Block("GrExtras Divide"),
        _vlen(vlen)
    {
        this->input_config(0).item_size = sizeof(type)*_vlen;
        this->output_config(0).item_size = sizeof(type)*_vlen;
    }

    void notify_topology(const size_t num_inputs, const size_t num_outputs)
    {
        for (size_t i = 0; i < num_inputs; i++)
        {
            this->input_config(i).inline_buffer = (i == 0);
        }
    }

    void work(const InputItems &, const OutputItems &);

    const size_t _vlen;
};

/***********************************************************************
 * Generic Divider implementation
 **********************************************************************/
template <typename type>
void Divide<type>::work(
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
static gras::Block *make_divide_fc32_fc32(const size_t &vlen)
{
    return new Divide<std::complex<float> >(vlen);
}

static gras::Block *make_divide_sc32_sc32(const size_t &vlen)
{
    return new Divide<std::complex<boost::int32_t> >(vlen);
}

static gras::Block *make_divide_sc16_sc16(const size_t &vlen)
{
    return new Divide<std::complex<boost::int16_t> >(vlen);
}

static gras::Block *make_divide_sc8_sc8(const size_t &vlen)
{
    return new Divide<std::complex<boost::int8_t> >(vlen);
}

static gras::Block *make_divide_f32_f32(const size_t &vlen)
{
    return new Divide<float>(vlen);
}

static gras::Block *make_divide_s32_s32(const size_t &vlen)
{
    return new Divide<boost::int32_t>(vlen);
}

static gras::Block *make_divide_s16_s16(const size_t &vlen)
{
    return new Divide<boost::int16_t>(vlen);
}

static gras::Block *make_divide_s8_s8(const size_t &vlen)
{
    return new Divide<boost::int8_t>(vlen);
}

GRAS_REGISTER_FACTORY("/extras/divide_fc32_fc32", make_divide_fc32_fc32)
GRAS_REGISTER_FACTORY("/extras/divide_sc32_sc32", make_divide_sc32_sc32)
GRAS_REGISTER_FACTORY("/extras/divide_sc16_sc16", make_divide_sc16_sc16)
GRAS_REGISTER_FACTORY("/extras/divide_sc8_sc8", make_divide_sc8_sc8)
GRAS_REGISTER_FACTORY("/extras/divide_f32_f32", make_divide_f32_f32)
GRAS_REGISTER_FACTORY("/extras/divide_s32_sc2", make_divide_s32_s32)
GRAS_REGISTER_FACTORY("/extras/divide_s16_s16", make_divide_s16_s16)
GRAS_REGISTER_FACTORY("/extras/divide_s8_s8", make_divide_s8_s8)
