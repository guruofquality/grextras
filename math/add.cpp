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
        this->register_call("set_preload", &Add::set_preload);
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
#define make_factory_function(suffix, type, factor) \
static gras::Block *make_add_v_ ## suffix(const size_t &vlen) \
{ \
    return new Add<type>(vlen*factor); \
} \
GRAS_REGISTER_FACTORY("/extras/add_v_" #suffix, make_add_v_ ## suffix) \
static gras::Block *make_add_ ## suffix(void) \
{ \
    return new Add<type>(1*factor); \
} \
GRAS_REGISTER_FACTORY("/extras/add_" #suffix, make_add_ ## suffix)

make_factory_function(fc32_fc32, float, 2) //factor of 2 to reuse float converter
make_factory_function(sc32_sc32, std::complex<boost::int32_t>, 1)
make_factory_function(sc16_sc16, std::complex<boost::int16_t>, 1)
make_factory_function(sc8_sc8, std::complex<boost::int8_t>, 1)
make_factory_function(f32_f32, float, 1)
make_factory_function(s32_s32, boost::int32_t, 1)
make_factory_function(s16_s16, boost::int16_t, 1)
make_factory_function(s8_s8, boost::int8_t, 1)
