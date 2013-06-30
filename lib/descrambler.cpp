// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#include "lfsr.h"
#include <grextras/descrambler.hpp>
#include <boost/cstdint.hpp>
#include <boost/foreach.hpp>
#include <stdexcept>

using namespace grextras;

struct DescramblerImpl : gras::Block
{
    DescramblerImpl(void):
        gras::Block("GrExtras Scrambler"),
        _polynom(1), _seed_value(1)
    {
        std::memset(&_lfsr, 0, sizeof(_lfsr));
        this->register_setter("polynomial", &DescramblerImpl::set_polynomial);
        this->register_setter("seed", &DescramblerImpl::set_seed);
        this->register_setter("mode", &DescramblerImpl::set_mode);
        this->register_setter("sync_word", &DescramblerImpl::set_sync_word);

        //some defaults
        //...

    }

    void set_polynomial(const boost::int64_t &polynomial)
    {
        _polynom = polynomial;
        GLFSR_init(&_lfsr, _polynom, _seed_value);
    }
    void set_seed(const boost::int64_t &seed)
    {
        _seed_value = seed;
        GLFSR_init(&_lfsr, _polynom, _seed_value);
    }
    void set_mode(const std::string &mode)
    {
        if (mode == "additive") _mode = MODE_ADD;
        else if (mode == "multiplicative") _mode = MODE_MULT;
        else throw std::invalid_argument("Descrambler: unknown mode: " + mode);
    }
    void set_sync_word(const std::string &sync_word)
    {
        _sync_word = sync_word;
    }

    void work(const InputItems &, const OutputItems &);
    unsigned char additive_bit_work(const unsigned char in);
    unsigned char multiplicative_bit_work(const unsigned char in);

    lfsr_t _lfsr;
    lfsr_data_t _polynom;
    lfsr_data_t _seed_value;
    enum {MODE_ADD, MODE_MULT} _mode;
    std::string _sync_word;
    long _count_down_to_sync_word;
};

unsigned char DescramblerImpl::additive_bit_work(const unsigned char in)
{
    const unsigned char ret = GLFSR_next(&_lfsr);
    const unsigned char out = in ^ ret;
    return out;
}

unsigned char DescramblerImpl::multiplicative_bit_work(const unsigned char in)
{
    const unsigned char ret = GLFSR_next(&_lfsr);
    const unsigned char out = in ^ ret;
    //input bit becomes the next bit0
    _lfsr.data &= ~lfsr_data_t(0x1);
    _lfsr.data |= in;
    return out;
}

void DescramblerImpl::work(const InputItems &ins, const OutputItems &outs)
{
    const size_t n = std::min(ins.min(), outs.min());
    const unsigned char *in = ins[0].cast<const unsigned char *>();
    unsigned char *out = outs[0].cast<unsigned char *>();

    //TODO look for sync word

    if (_mode == MODE_ADD)
    {
        for (size_t i = 0; i < n; i++)
        {
            out[i] = this->additive_bit_work(in[i] & 0x1);
        }
    }
    if (_mode == MODE_MULT)
    {
        for (size_t i = 0; i < n; i++)
        {
            out[i] = this->multiplicative_bit_work(in[i] & 0x1);
        }
    }

    this->consume(n);
    this->produce(n);
}

gras::Block *Descrambler::make(void)
{
    return new DescramblerImpl();
}
