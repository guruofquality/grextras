// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#include <gras/block.hpp>
#include <gras/factory.hpp>
#include <stdexcept>
#include <complex>
#ifdef HAVE_VOLK
#include <volk/volk.h>
#endif

/***********************************************************************
 * Templated Adder class
 **********************************************************************/
template <typename type>
struct Add : gras::Block
{
    Add(const size_t vlen):
        gras::Block("GrExtras Add"),
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
 * Generic Adder implementation
 **********************************************************************/
template <typename type>
void Add<type>::work(
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
void Add<float>::work(
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
static gras::Block *make_add_fc32_fc32(const size_t &vlen)
{
    return new Add<float>(2*vlen);
}

static gras::Block *make_add_sc32_sc32(const size_t &vlen)
{
    return new Add<boost::int32_t>(2*vlen);
}

static gras::Block *make_add_sc16_sc16(const size_t &vlen)
{
    return new Add<boost::int16_t>(2*vlen);
}

static gras::Block *make_add_sc8_sc8(const size_t &vlen)
{
    return new Add<boost::int8_t>(2*vlen);
}

static gras::Block *make_add_f32_f32(const size_t &vlen)
{
    return new Add<float>(vlen);
}

static gras::Block *make_add_s32_s32(const size_t &vlen)
{
    return new Add<boost::int32_t>(vlen);
}

static gras::Block *make_add_s16_s16(const size_t &vlen)
{
    return new Add<boost::int16_t>(vlen);
}

static gras::Block *make_add_s8_s8(const size_t &vlen)
{
    return new Add<boost::int8_t>(vlen);
}

GRAS_REGISTER_FACTORY("/extras/add_fc32_fc32", make_add_fc32_fc32)
GRAS_REGISTER_FACTORY("/extras/add_sc32_sc32", make_add_sc32_sc32)
GRAS_REGISTER_FACTORY("/extras/add_sc16_sc16", make_add_sc16_sc16)
GRAS_REGISTER_FACTORY("/extras/add_sc8_sc8", make_add_sc8_sc8)
GRAS_REGISTER_FACTORY("/extras/add_f32_f32", make_add_f32_f32)
GRAS_REGISTER_FACTORY("/extras/add_s32_sc2", make_add_s32_s32)
GRAS_REGISTER_FACTORY("/extras/add_s16_s16", make_add_s16_s16)
GRAS_REGISTER_FACTORY("/extras/add_s8_s8", make_add_s8_s8)
