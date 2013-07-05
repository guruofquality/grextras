// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#include <grextras/add_const.hpp>
#include <boost/make_shared.hpp>
#include <stdexcept>

using namespace grextras;

//! This class is nothing more than a wrapper for the vlen=1 case of AddConstV

class AddConstImpl : public AddConst
{
public:

    AddConstImpl(const size_t size, AddConstV::sptr ub):
        gras::HierBlock("GrExtras Add Const"),
        underlying_block(ub)
    {
        this->connect(*this, 0, ub, 0);
        this->connect(ub, 0, *this, 0);
    }

    void set_const(const std::complex<double> &val)
    {
        underlying_block->set("const",
            std::vector<std::complex<double> >(1, val));
    }

    std::complex<double> get_const(void)
    {
        return underlying_block->get<std::vector<std::complex<double> > >("const").front();
    }

private:
    AddConstV::sptr underlying_block;
};

#define make_factory_function(suffix, type, op) \
    AddConst::sptr AddConst::make_ ## suffix( \
        const std::complex<double> &val \
    ){ \
        return boost::make_shared<AddConstImpl>( \
            sizeof(type), \
            AddConstV::make_ ## suffix( \
            std::vector<type>(1, type(val op)))); \
    }

make_factory_function(fc32_fc32, std::complex<float>, )
make_factory_function(sc32_sc32, std::complex<boost::int32_t>, )
make_factory_function(sc16_sc16, std::complex<boost::int16_t>, )
make_factory_function(sc8_sc8, std::complex<boost::int8_t>, )
make_factory_function(f32_f32, float, .real())
make_factory_function(s32_s32, boost::int32_t, .real())
make_factory_function(s16_s16, boost::int16_t, .real())
make_factory_function(s8_s8, boost::int8_t, .real())
