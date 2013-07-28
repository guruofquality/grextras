// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#include <gras/block.hpp>
#include <gras/factory.hpp>
#include <stdexcept>
#include <complex>
#include <volk/volk.h>

/***********************************************************************
 * Templated Subtractor class
 **********************************************************************/
template <typename type>
struct Subtract : gras::Block
{
    Subtract(const size_t vlen):
        gras::Block("GrExtras Subtract"),
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
 * Generic Subtractor implementation
 **********************************************************************/
template <typename type>
void Subtract<type>::work(
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
static gras::Block *make_subtract_fc32_fc32(const size_t &vlen)
{
    return new Subtract<std::complex<float> >(vlen);
}

static gras::Block *make_subtract_sc32_sc32(const size_t &vlen)
{
    return new Subtract<std::complex<boost::int32_t> >(vlen);
}

static gras::Block *make_subtract_sc16_sc16(const size_t &vlen)
{
    return new Subtract<std::complex<boost::int16_t> >(vlen);
}

static gras::Block *make_subtract_sc8_sc8(const size_t &vlen)
{
    return new Subtract<std::complex<boost::int8_t> >(vlen);
}

static gras::Block *make_subtract_f32_f32(const size_t &vlen)
{
    return new Subtract<float>(vlen);
}

static gras::Block *make_subtract_s32_s32(const size_t &vlen)
{
    return new Subtract<boost::int32_t>(vlen);
}

static gras::Block *make_subtract_s16_s16(const size_t &vlen)
{
    return new Subtract<boost::int16_t>(vlen);
}

static gras::Block *make_subtract_s8_s8(const size_t &vlen)
{
    return new Subtract<boost::int8_t>(vlen);
}

GRAS_REGISTER_FACTORY("/extras/subtract_fc32_fc32", make_subtract_fc32_fc32)
GRAS_REGISTER_FACTORY("/extras/subtract_sc32_sc32", make_subtract_sc32_sc32)
GRAS_REGISTER_FACTORY("/extras/subtract_sc16_sc16", make_subtract_sc16_sc16)
GRAS_REGISTER_FACTORY("/extras/subtract_sc8_sc8", make_subtract_sc8_sc8)
GRAS_REGISTER_FACTORY("/extras/subtract_f32_f32", make_subtract_f32_f32)
GRAS_REGISTER_FACTORY("/extras/subtract_s32_sc2", make_subtract_s32_s32)
GRAS_REGISTER_FACTORY("/extras/subtract_s16_s16", make_subtract_s16_s16)
GRAS_REGISTER_FACTORY("/extras/subtract_s8_s8", make_subtract_s8_s8)
