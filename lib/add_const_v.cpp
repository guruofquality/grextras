// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#include <grextras/add_const.hpp>
#include <boost/make_shared.hpp>
#include <stdexcept>

using namespace grextras;

/***********************************************************************
 * Generic multiply const implementation
 **********************************************************************/
template <typename type>
class AddConstVImpl : public AddConstV
{
public:
    AddConstVImpl(const std::vector<type> &vec):
        gras::Block("GrExtras AddConst")
    {
        this->set_input_signature(gras::IOSignature(sizeof(type)*vec.size()));
        this->set_output_signature(gras::IOSignature(sizeof(type)*vec.size()));
        _val.resize(vec.size());
        this->set_const(vec);
    }

    void work1(const InputItems &, const OutputItems &);

    void work(
        const InputItems &ins,
        const OutputItems &outs
    ){
        if (_val.size() == 1) return work1(ins, outs);

        const size_t n_nums = std::min(ins.min(), outs.min());
        type *out = outs[0].cast<type *>();
        const type *in = ins[0].cast<const type *>();

        for (size_t i = 0; i < n_nums * _val.size(); i++)
        {
            out[i] = in[i] + _val[i%_val.size()];
        }

        this->consume(n_nums);
        this->produce(n_nums);
    }

    void _set_const(const std::vector<std::complex<double> > &val)
    {
        _original_val.assign(val.begin(), val.end());

        if (val.size() != _val.size()){
            throw std::invalid_argument("set_const called with the wrong length");
        }
        for (size_t i = 0; i < val.size(); i++){
            complex128_to_num(val[i], _val[i]);
        }
    }

    std::vector<std::complex<double> > get_const(void)
    {
        return _original_val;
    }

private:
    std::vector<std::complex<double> > _original_val;
    std::vector<type> _val;
};

/***********************************************************************
 * Generic vlen == 1 multiplier implementation
 **********************************************************************/
template <typename type>
void AddConstVImpl<type>::work1(
    const InputItems &ins, const OutputItems &outs
){
    const size_t n_nums = std::min(ins.min(), outs.min());
    type *out = outs[0].cast<type *>();
    const type *in = ins[0].cast<const type *>();

    for (size_t i = 0; i < n_nums; i++)
    {
        out[i] = in[i] + _val[0];
    }

    this->consume(n_nums);
    this->produce(n_nums);
}

/***********************************************************************
 * factory function
 **********************************************************************/
#define make_factory_function(suffix, type) \
    AddConstV::sptr AddConstV::make_ ## suffix(const std::vector<type > &vec){ \
    return sptr(new AddConstVImpl<type>(vec)); \
}

make_factory_function(fc32_fc32, std::complex<float>)
make_factory_function(sc32_sc32, std::complex<boost::int32_t>)
make_factory_function(sc16_sc16, std::complex<boost::int16_t>)
make_factory_function(sc8_sc8, std::complex<boost::int8_t>)
make_factory_function(f32_f32, float)
make_factory_function(s32_s32, boost::int32_t)
make_factory_function(s16_s16, boost::int16_t)
make_factory_function(s8_s8, boost::int8_t)
