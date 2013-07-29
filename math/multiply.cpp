// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#include <gras/block.hpp>
#include <gras/factory.hpp>
#include <stdexcept>
#include <complex>
#include <boost/cstdint.hpp>
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
        this->register_call("set_preload", &Multiply::set_preload);
    }

    void set_preload(const std::vector<size_t> &preload)
    {
        for (size_t i = 0; i < preload.size(); i++)
        {
            this->input_config(i).preload_items = preload[i];
        }
        this->commit_config();
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
#define make_factory_function(suffix, type) \
static gras::Block *make_multiply_v_ ## suffix(const size_t &vlen) \
{ \
    return new Multiply<type>(vlen); \
} \
GRAS_REGISTER_FACTORY("/extras/multiply_v_" #suffix, make_multiply_v_ ## suffix) \
static gras::Block *make_multiply_ ## suffix(void) \
{ \
    return new Multiply<type>(1); \
} \
GRAS_REGISTER_FACTORY("/extras/multiply_" #suffix, make_multiply_ ## suffix)

make_factory_function(fc32_fc32, std::complex<float>)
make_factory_function(sc32_sc32, std::complex<boost::int32_t>)
make_factory_function(sc16_sc16, std::complex<boost::int16_t>)
make_factory_function(sc8_sc8, std::complex<boost::int8_t>)
make_factory_function(f32_f32, float)
make_factory_function(s32_s32, boost::int32_t)
make_factory_function(s16_s16, boost::int16_t)
make_factory_function(s8_s8, boost::int8_t)
