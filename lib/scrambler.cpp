// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#include "lfsr.h"
#include <grextras/scrambler.hpp>
#include <boost/cstdint.hpp>
#include <boost/foreach.hpp>
#include <stdexcept>
#include <iostream>

using namespace grextras;

struct ScramblerImpl : gras::Block
{
    ScramblerImpl(void):
        gras::Block("GrExtras Scrambler"),
        _polynom(1), _seed_value(1)
    {
        std::memset(&_lfsr, 0, sizeof(_lfsr));
        this->register_setter("polynomial", &ScramblerImpl::set_polynomial);
        this->register_setter("seed", &ScramblerImpl::set_seed);
        this->register_setter("mode", &ScramblerImpl::set_mode);
        this->register_setter("sync_word", &ScramblerImpl::set_sync_word);
        this->register_setter("io_type", &ScramblerImpl::set_io_type);

        //some defaults
        //...

    }

    void notify_active(void)
    {
        //sync word should be tiny, so this is really done for completeness
        this->output_config(0).reserve_items = _sync_word.size()*2;
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
        else throw std::invalid_argument("Scrambler: unknown mode: " + mode);
    }
    void set_sync_word(const std::string &sync_word)
    {
        _sync_word = sync_word;
    }
    void set_io_type(const std::string &io_type)
    {
        if (io_type == "bits") _io_type = IO_TYPE_BITS;
        else if (io_type == "msb_bytes") _io_type = IO_TYPE_MSB_BYTES;
        else if (io_type == "lsb_bytes") _io_type = IO_TYPE_LSB_BYTES;
        else throw std::invalid_argument("Scrambler: unknown IO type: " + io_type);
    }

    void work(const InputItems &, const OutputItems &);
    unsigned char additive_bit_work(const unsigned char in);
    unsigned char multiplicative_bit_work(const unsigned char in);

    lfsr_t _lfsr;
    lfsr_data_t _polynom;
    lfsr_data_t _seed_value;
    enum {MODE_ADD, MODE_MULT} _mode;
    enum {IO_TYPE_BITS, IO_TYPE_MSB_BYTES, IO_TYPE_LSB_BYTES} _io_type;
    std::string _sync_word;
    long _count_down_to_sync_word;
};

unsigned char ScramblerImpl::additive_bit_work(const unsigned char in)
{
    const unsigned char ret = GLFSR_next(&_lfsr);
    const unsigned char out = in ^ ret;
    return out;
}

unsigned char ScramblerImpl::multiplicative_bit_work(const unsigned char in)
{
    const unsigned char ret = GLFSR_next(&_lfsr);
    const unsigned char out = in ^ ret;
    //output bit becomes the next bit0
    _lfsr.data &= ~lfsr_data_t(0x1);
    _lfsr.data |= out;
    return out;
}

void ScramblerImpl::work(const InputItems &ins, const OutputItems &outs)
{
    const size_t n = std::min(ins.min(), outs.min());
    const unsigned char *in = ins[0].cast<const unsigned char *>();
    unsigned char *out = outs[0].cast<unsigned char *>();

    //find length tag and send sync word
    BOOST_FOREACH(gras::Tag t, this->get_input_tags(0))
    {
        //filter out tags past the available input
        if (t.offset >= this->get_consumed(0) + n) continue;

        //filter out non length tags
        if (not t.object.is<gras::StreamTag>()) continue;
        PMCC key = t.object.as<gras::StreamTag>().key;
        PMCC val = t.object.as<gras::StreamTag>().val;
        if (not key.is<std::string>()) continue;
        if (key.as<std::string>() != "length") continue;
        if (not val.is<size_t>()) continue;

        const size_t &length = val.as<size_t>();
        //TODO handle me

    }

    //no tags in this next range
    if (_mode == MODE_ADD)
    {
        if (_io_type == IO_TYPE_MSB_BYTES)
        {
            for (size_t i = 0; i < n; i++)
            {
                out[i] = 0;
                out[i] |= this->additive_bit_work((in[i] >> 7) & 0x1) << 7;
                out[i] |= this->additive_bit_work((in[i] >> 6) & 0x1) << 6;
                out[i] |= this->additive_bit_work((in[i] >> 5) & 0x1) << 5;
                out[i] |= this->additive_bit_work((in[i] >> 4) & 0x1) << 4;
                out[i] |= this->additive_bit_work((in[i] >> 3) & 0x1) << 3;
                out[i] |= this->additive_bit_work((in[i] >> 2) & 0x1) << 2;
                out[i] |= this->additive_bit_work((in[i] >> 1) & 0x1) << 1;
                out[i] |= this->additive_bit_work((in[i] >> 0) & 0x1) << 0;
            }
        }
        if (_io_type == IO_TYPE_LSB_BYTES)
        {
            for (size_t i = 0; i < n; i++)
            {
                out[i] = 0;
                out[i] |= this->additive_bit_work((in[i] >> 0) & 0x1) << 0;
                out[i] |= this->additive_bit_work((in[i] >> 1) & 0x1) << 1;
                out[i] |= this->additive_bit_work((in[i] >> 2) & 0x1) << 2;
                out[i] |= this->additive_bit_work((in[i] >> 3) & 0x1) << 3;
                out[i] |= this->additive_bit_work((in[i] >> 4) & 0x1) << 4;
                out[i] |= this->additive_bit_work((in[i] >> 5) & 0x1) << 5;
                out[i] |= this->additive_bit_work((in[i] >> 6) & 0x1) << 6;
                out[i] |= this->additive_bit_work((in[i] >> 7) & 0x1) << 7;
            }
        }
        if (_io_type == IO_TYPE_BITS)
        {
            for (size_t i = 0; i < n; i++)
            {
                out[i] = this->additive_bit_work(in[i] & 0x1);
            }
        }
    }
    if (_mode == MODE_MULT)
    {
        if (_io_type == IO_TYPE_MSB_BYTES)
        {
            for (size_t i = 0; i < n; i++)
            {
                out[i] = 0;
                out[i] |= this->multiplicative_bit_work((in[i] >> 7) & 0x1) << 7;
                out[i] |= this->multiplicative_bit_work((in[i] >> 6) & 0x1) << 6;
                out[i] |= this->multiplicative_bit_work((in[i] >> 5) & 0x1) << 5;
                out[i] |= this->multiplicative_bit_work((in[i] >> 4) & 0x1) << 4;
                out[i] |= this->multiplicative_bit_work((in[i] >> 3) & 0x1) << 3;
                out[i] |= this->multiplicative_bit_work((in[i] >> 2) & 0x1) << 2;
                out[i] |= this->multiplicative_bit_work((in[i] >> 1) & 0x1) << 1;
                out[i] |= this->multiplicative_bit_work((in[i] >> 0) & 0x1) << 0;
            }
        }
        if (_io_type == IO_TYPE_LSB_BYTES)
        {
            for (size_t i = 0; i < n; i++)
            {
                out[i] = 0;
                out[i] |= this->multiplicative_bit_work((in[i] >> 0) & 0x1) << 0;
                out[i] |= this->multiplicative_bit_work((in[i] >> 1) & 0x1) << 1;
                out[i] |= this->multiplicative_bit_work((in[i] >> 2) & 0x1) << 2;
                out[i] |= this->multiplicative_bit_work((in[i] >> 3) & 0x1) << 3;
                out[i] |= this->multiplicative_bit_work((in[i] >> 4) & 0x1) << 4;
                out[i] |= this->multiplicative_bit_work((in[i] >> 5) & 0x1) << 5;
                out[i] |= this->multiplicative_bit_work((in[i] >> 6) & 0x1) << 6;
                out[i] |= this->multiplicative_bit_work((in[i] >> 7) & 0x1) << 7;
            }
        }
        if (_io_type == IO_TYPE_BITS)
        {
            for (size_t i = 0; i < n; i++)
            {
                out[i] = this->multiplicative_bit_work(in[i] & 0x1);
            }
        }
    }

    this->consume(n);
    this->produce(n);
}

gras::Block *Scrambler::make(void)
{
    return new ScramblerImpl();
}
