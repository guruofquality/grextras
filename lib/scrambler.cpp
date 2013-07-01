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

        //some defaults
        this->set_mode("multiplicative");
        this->set_sync_word("");
        this->set_polynomial(8650753);
    }

    void notify_active(void)
    {
        if (_sync_word.empty())
        {
            this->output_config(0).reserve_items = 1;
        }
        else
        {
            this->output_config(0).reserve_items = _sync_word.size() + 1;
        }
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
        _sync_bits.clear();
        for (size_t i = 0; i < _sync_word.size(); i++)
        {
            if (_sync_word[i] == '0')
            {
                _sync_bits.push_back(0);
            }
            else if (_sync_word[i] == '1')
            {
                _sync_bits.push_back(1);
            }
            else throw std::out_of_range("Scrambler: sync word must be 0s and 1s: " + _sync_word);
        }
    }

    void work(const InputItems &, const OutputItems &);
    unsigned char additive_bit_work(const unsigned char in);
    unsigned char multiplicative_bit_work(const unsigned char in);

    lfsr_t _lfsr;
    lfsr_data_t _polynom;
    lfsr_data_t _seed_value;
    enum {MODE_ADD, MODE_MULT} _mode;
    std::string _sync_word;
    std::vector<unsigned char> _sync_bits;
    long _count_down_to_sync_word;
};

GRAS_FORCE_INLINE unsigned char ScramblerImpl::additive_bit_work(const unsigned char in)
{
    const unsigned char ret = GLFSR_next(&_lfsr);
    const unsigned char out = in ^ ret;
    return out;
}

GRAS_FORCE_INLINE unsigned char ScramblerImpl::multiplicative_bit_work(const unsigned char in)
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
    size_t n = std::min(ins.min(), outs.min());
    const unsigned char *in = ins[0].cast<const unsigned char *>();
    unsigned char *out = outs[0].cast<unsigned char *>();

    //find length tag and send sync word
    BOOST_FOREACH(gras::Tag t, this->get_input_tags(0))
    {
        //dont operate loop without sync word
        if (_sync_word.empty()) continue;

        //filter out tags past the available input
        if (t.offset >= this->get_consumed(0) + n) continue;

        //filter out non length tags
        if (not t.object.is<gras::StreamTag>()) continue;
        PMCC key = t.object.as<gras::StreamTag>().key;
        PMCC val = t.object.as<gras::StreamTag>().val;
        if (not key.is<std::string>()) continue;
        if (key.as<std::string>() != "length") continue;
        if (not val.is<size_t>()) continue;

        //only want to deal with length tags at index 0
        if (t.offset == this->get_consumed(0))
        {
            //reset the lfsr to seed state
            GLFSR_init(&_lfsr, _polynom, _seed_value);

            //copy in the sync word and produce bits
            std::copy(_sync_bits.begin(), _sync_bits.end(), &out[0]);
            this->produce(_sync_word.size());

            //advance out and decrement n for work loop
            out += _sync_word.size();
            const size_t &length = val.as<size_t>();
            n = std::min(n - _sync_word.size(), length);
        }
        //otherwise go up to but not including length tag
        else
        {
            n = t.offset - this->get_consumed(0);
            break;
        }
    }

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
}

gras::Block *Scrambler::make(void)
{
    return new ScramblerImpl();
}
