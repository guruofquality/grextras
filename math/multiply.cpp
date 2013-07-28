// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#include <gras/block.hpp>
#include <gras/factory.hpp>
#include <stdexcept>
#include <complex>
#ifdef HAVE_VOLK
#include <volk/volk.h>
#endif

/***********************************************************************
 * Templated Multiplier class
 **********************************************************************/
template <typename type>
struct Multiply : gras::Block
{
    Multiply(const size_t vlen):
        gras::Block("GrExtras Multiply"),
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
 * Generic Multiplier implementation
 **********************************************************************/
template <typename type>
void Multiply<type>::work(
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
            out[i] = in0[i] * in[i];
        }
        in0 = out; //for next input, we do output *= input
    }

    this->consume(n_nums);
    this->produce(n_nums);
}

#ifdef HAVE_VOLK
/***********************************************************************
 * Multiplier implementation with complex complex float32 - calls volk
 **********************************************************************/
template <>
void Multiply<std::complex<float> >::work(
    const InputItems &ins, const OutputItems &outs
){
    const size_t n_nums = std::min(ins.min(), outs.min());
    std::complex<float> *out = outs[0].cast<std::complex<float> *>();
    const std::complex<float> *in0 = ins[0].cast<const std::complex<float> *>();

    for (size_t n = 1; n < ins.size(); n++)
    {
        const std::complex<float> *in = ins[n].cast<const std::complex<float> *>();
        volk_32fc_x2_multiply_32fc(out, in0, in, n_nums * _vlen);
        in0 = out; //for next input, we do output *= input
    }

    this->consume(n_nums);
    this->produce(n_nums);
}

/***********************************************************************
 * Multiplier implementation with float32 - calls volk
 **********************************************************************/
template <>
void Multiply<float>::work(
    const InputItems &ins, const OutputItems &outs
){
    const size_t n_nums = std::min(ins.min(), outs.min());
    float *out = outs[0].cast<float *>();
    const float *in0 = ins[0].cast<const float *>();

    for (size_t n = 1; n < ins.size(); n++)
    {
        const float *in = ins[n].cast<const float *>();
        volk_32f_x2_multiply_32f(out, in0, in, n_nums * _vlen);
        in0 = out; //for next input, we do output *= input
    }

    this->consume(n_nums);
    this->produce(n_nums);
}
#endif

/***********************************************************************
 * factory function
 **********************************************************************/
static gras::Block *make_multiply_fc32_fc32(const size_t &vlen)
{
    return new Multiply<std::complex<float> >(vlen);
}

static gras::Block *make_multiply_sc32_sc32(const size_t &vlen)
{
    return new Multiply<std::complex<boost::int32_t> >(vlen);
}

static gras::Block *make_multiply_sc16_sc16(const size_t &vlen)
{
    return new Multiply<std::complex<boost::int16_t> >(vlen);
}

static gras::Block *make_multiply_sc8_sc8(const size_t &vlen)
{
    return new Multiply<std::complex<boost::int8_t> >(vlen);
}

static gras::Block *make_multiply_f32_f32(const size_t &vlen)
{
    return new Multiply<float>(vlen);
}

static gras::Block *make_multiply_s32_s32(const size_t &vlen)
{
    return new Multiply<boost::int32_t>(vlen);
}

static gras::Block *make_multiply_s16_s16(const size_t &vlen)
{
    return new Multiply<boost::int16_t>(vlen);
}

static gras::Block *make_multiply_s8_s8(const size_t &vlen)
{
    return new Multiply<boost::int8_t>(vlen);
}

GRAS_REGISTER_FACTORY("/extras/multiply_fc32_fc32", make_multiply_fc32_fc32)
GRAS_REGISTER_FACTORY("/extras/multiply_sc32_sc32", make_multiply_sc32_sc32)
GRAS_REGISTER_FACTORY("/extras/multiply_sc16_sc16", make_multiply_sc16_sc16)
GRAS_REGISTER_FACTORY("/extras/multiply_sc8_sc8", make_multiply_sc8_sc8)
GRAS_REGISTER_FACTORY("/extras/multiply_f32_f32", make_multiply_f32_f32)
GRAS_REGISTER_FACTORY("/extras/multiply_s32_sc2", make_multiply_s32_s32)
GRAS_REGISTER_FACTORY("/extras/multiply_s16_s16", make_multiply_s16_s16)
GRAS_REGISTER_FACTORY("/extras/multiply_s8_s8", make_multiply_s8_s8)
