// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#ifndef INCLUDED_GREXTRAS_CONFIG_HPP
#define INCLUDED_GREXTRAS_CONFIG_HPP

#include <ciso646>

// http://gcc.gnu.org/wiki/Visibility
// Generic helper definitions for shared library support
#if defined _WIN32 || defined __CYGWIN__
  #define GREXTRAS_HELPER_DLL_IMPORT __declspec(dllimport)
  #define GREXTRAS_HELPER_DLL_EXPORT __declspec(dllexport)
  #define GREXTRAS_HELPER_DLL_LOCAL
#else
  #if __GNUC__ >= 4
    #define GREXTRAS_HELPER_DLL_IMPORT __attribute__ ((visibility ("default")))
    #define GREXTRAS_HELPER_DLL_EXPORT __attribute__ ((visibility ("default")))
    #define GREXTRAS_HELPER_DLL_LOCAL  __attribute__ ((visibility ("hidden")))
  #else
    #define GREXTRAS_HELPER_DLL_IMPORT
    #define GREXTRAS_HELPER_DLL_EXPORT
    #define GREXTRAS_HELPER_DLL_LOCAL
  #endif
#endif

#define GREXTRAS_DLL //always build a dll

// Now we use the generic helper definitions above to define GREXTRAS_API and GREXTRAS_LOCAL.
// GREXTRAS_API is used for the public API symbols. It either DLL imports or DLL exports (or does nothing for static build)
// GREXTRAS_LOCAL is used for non-api symbols.

#ifdef GREXTRAS_DLL // defined if GRAS is compiled as a DLL
  #ifdef GREXTRAS_DLL_EXPORTS // defined if we are building the GRAS DLL (instead of using it)
    #define GREXTRAS_API GREXTRAS_HELPER_DLL_EXPORT
  #else
    #define GREXTRAS_API GREXTRAS_HELPER_DLL_IMPORT
  #endif // GREXTRAS_DLL_EXPORTS
  #define GREXTRAS_LOCAL GREXTRAS_HELPER_DLL_LOCAL
#else // GREXTRAS_DLL is not defined: this means GRAS is a static lib.
  #define GREXTRAS_API
  #define GREXTRAS_LOCAL
#endif // GREXTRAS_DLL

#include <gras/block.hpp>
#include <boost/foreach.hpp>

namespace grextras
{

struct SyncBlock : gras::Block
{
    SyncBlock(void):
        gras::Block()
    {
        //NOP
    }

    SyncBlock(const std::string &name):
        gras::Block(name)
    {
        this->set_relative_rate(1.0);
    }

    void set_relative_rate(const double rate)
    {
        _relative_rate = rate;
    }

    inline void work(
        const InputItems &input_items,
        const OutputItems &output_items
    )
    {
        _input_items = input_items;
        _output_items = output_items;
        const size_t num_inputs = std::min(_input_items.min_items(), size_t(_output_items.min_items()/_relative_rate));
        const size_t num_outputs = std::min(size_t(_input_items.min_items()*_relative_rate), _output_items.min_items());

        for (size_t i = 0; i < _input_items.size(); i++)
        {
            _input_items[i].size() = num_inputs;
        }

        for (size_t o = 0; o < _output_items.size(); o++)
        {
            _output_items[o].size() = num_outputs;
        }

        const size_t items = this->sync_work(_input_items, _output_items);

        for (size_t i = 0; i < _input_items.size(); i++)
        {
            this->consume(i, items/_relative_rate);
        }

        for (size_t o = 0; o < _output_items.size(); o++)
        {
            this->produce(o, items);
        }
    }

    virtual size_t sync_work(
        const InputItems &input_items,
        const OutputItems &output_items
    ) = 0;

    inline virtual void propagate_tags(const size_t i, const gras::TagIter &iter)
    {
        for (size_t o = 0; o < _output_items.size(); o++)
        {
            BOOST_FOREACH(gras::Tag t, iter)
            {
                t.offset -= this->get_consumed(i);
                t.offset *= _relative_rate;
                t.offset += this->get_produced(o);
                this->post_output_tag(o, t);
            }
        }
    }

    InputItems _input_items;
    OutputItems _output_items;
    double _relative_rate;
};

}

#endif /*INCLUDED_GREXTRAS_CONFIG_HPP*/
