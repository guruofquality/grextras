// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#include "lfsr.h"
#include <gras/block.hpp>
#include <gras/factory.hpp>
#include <boost/cstdint.hpp>
#include <boost/foreach.hpp>
#include <stdexcept>
#include <iostream>

struct Descrambler : gras::Block
{
    Descrambler(void):
        gras::Block("GrExtras Scrambler"),
        _polynom(1), _seed_value(1)
    {
        std::memset(&_lfsr, 0, sizeof(_lfsr));
        this->register_call("set_poly", &Descrambler::set_poly);
        this->register_call("set_seed", &Descrambler::set_seed);
        this->register_call("set_mode", &Descrambler::set_mode);
        this->register_call("set_sync", &Descrambler::set_sync);

        //some defaults
        this->set_mode("multiplicative");
        this->set_sync("");
        this->set_poly(8650753);
    }

    void set_poly(const boost::int64_t &polynomial)
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
    void set_sync(const std::string &sync_word)
    {
        _sync_word = sync_word;
        if (_sync_word.size() > 64) throw std::out_of_range("Descrambler: sync word max len 64 bits");

        _sync_bits = 0;
        _sync_mask = 0;

        for (size_t i = 0; i < _sync_word.size(); i++)
        {
            _sync_bits <<= 1;
            _sync_mask <<= 1;
            _sync_mask |= 0x1;
            if (_sync_word[i] == '0')
            {
                _sync_bits |= 0x0;
            }
            else if (_sync_word[i] == '1')
            {
                _sync_bits |= 0x1;
            }
            else throw std::out_of_range("Descrambler: sync word must be 0s and 1s: " + _sync_word);
        }

        if (_sync_word.empty()) this->input_config(0).reserve_items = 1;
        else this->input_config(0).reserve_items = _sync_word.size();
    }

    void work(const InputItems &, const OutputItems &);
    unsigned char additive_bit_work(const unsigned char in);
    unsigned char multiplicative_bit_work(const unsigned char in);

    lfsr_t _lfsr;
    lfsr_data_t _polynom;
    lfsr_data_t _seed_value;
    enum {MODE_ADD, MODE_MULT} _mode;
    std::string _sync_word;
    boost::uint64_t _sync_bits;
    boost::uint64_t _sync_mask;
    long _count_down_to_sync_word;
};

GRAS_FORCE_INLINE unsigned char Descrambler::additive_bit_work(const unsigned char in)
{
    const unsigned char ret = GLFSR_next(&_lfsr);
    const unsigned char out = in ^ ret;
    return out;
}

GRAS_FORCE_INLINE unsigned char Descrambler::multiplicative_bit_work(const unsigned char in)
{
    const unsigned char ret = GLFSR_next(&_lfsr);
    const unsigned char out = in ^ ret;
    //input bit becomes the next bit0
    _lfsr.data &= ~lfsr_data_t(0x1);
    _lfsr.data |= in;
    return out;
}

void Descrambler::work(const InputItems &ins, const OutputItems &outs)
{
    size_t n = std::min(ins.min(), outs.min());
    const unsigned char *in = ins[0].cast<const unsigned char *>();
    unsigned char *out = outs[0].cast<unsigned char *>();

    //Search for the sync word in the available bits.
    //If found, reduce N to avoid converting the sync word
    bool sync_word_found = false;
    boost::uint64_t bits = 0;
    if (not _sync_word.empty()) for (size_t i = 0; i < n; i++)
    {
        bits <<= 1; bits |= (in[i] & 0x1);
        if ((bits & _sync_mask) == _sync_bits and (i+1) >= _sync_word.size())
        {
            sync_word_found = true;
            n = i + 1 - _sync_word.size();
            break;
        }
    }

    //ignore trailing history bits that could be at the start of a new sync word
    if (not _sync_word.empty() and not sync_word_found) n -= _sync_word.size() - 1;

    //The main work loop deals with input bit by bit.
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

    //a sync word was found? consume it
    if (sync_word_found)
    {
        this->consume(_sync_word.size());
        //reset the lfsr to seed state
        GLFSR_init(&_lfsr, _polynom, _seed_value);
    }
}

GRAS_REGISTER_FACTORY0("/extras/descrambler", Descrambler)
