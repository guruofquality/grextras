// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#include <grextras/multiply.hpp>
#include <boost/make_shared.hpp>
#include <stdexcept>
#include <complex>
#ifdef HAVE_VOLK
#include <volk/volk.h>
#endif

using namespace grextras;

/***********************************************************************
 * Templated Multiplier class
 **********************************************************************/
template <typename type>
struct MultiplyImpl : Multiply
{
    MultiplyImpl(const size_t vlen):
        gras::Block("GrExtras Multiply"),
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
 * Generic Multiplier implementation
 **********************************************************************/
template <typename type>
void MultiplyImpl<type>::work(
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
void MultiplyImpl<std::complex<float> >::work(
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
void MultiplyImpl<float>::work(
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
Multiply::sptr Multiply::make_fc32_fc32(const size_t vlen)
{
    return sptr(new MultiplyImpl<std::complex<float> >(vlen));
}

Multiply::sptr Multiply::make_sc32_sc32(const size_t vlen)
{
    return sptr(new MultiplyImpl<std::complex<boost::int32_t> >(vlen));
}

Multiply::sptr Multiply::make_sc16_sc16(const size_t vlen)
{
    return sptr(new MultiplyImpl<std::complex<boost::int16_t> >(vlen));
}

Multiply::sptr Multiply::make_sc8_sc8(const size_t vlen)
{
    return sptr(new MultiplyImpl<std::complex<boost::int8_t> >(vlen));
}

Multiply::sptr Multiply::make_f32_f32(const size_t vlen)
{
    return sptr(new MultiplyImpl<float>(vlen));
}

Multiply::sptr Multiply::make_s32_s32(const size_t vlen)
{
    return sptr(new MultiplyImpl<boost::int32_t>(vlen));
}

Multiply::sptr Multiply::make_s16_s16(const size_t vlen)
{
    return sptr(new MultiplyImpl<boost::int16_t>(vlen));
}

Multiply::sptr Multiply::make_s8_s8(const size_t vlen)
{
    return sptr(new MultiplyImpl<boost::int8_t>(vlen));
}

