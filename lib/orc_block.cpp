// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#include <grextras/orc_block.hpp>
#include <boost/make_shared.hpp>
#include <stdexcept>
#include <iostream>

using namespace grextras;

OrcBlockParams::OrcBlockParams(void)
{
    production_factor = 1.0;
    consumption_offset = 0;
}

#ifdef HAVE_ORC

#include <orc/orc.h>
#include <orc/orcparse.h>

/***********************************************************************
 * impl class definition
 **********************************************************************/
struct OrcBlockImpl : OrcBlock
{
    OrcBlockImpl(void)
    {
        _program = NULL;
    }

    ~OrcBlockImpl(void)
    {
        if (_program) free(_program);
    }

    OrcBlockParams &params(void)
    {
        return _params;
    }

    void set_program(
        const std::string &name,
        const std::string &source
    )
    {
        //always orc init before using orc
        orc_init();

        //call into the parser
        OrcProgram **programs;
        char *log = NULL;
        std::cerr << "ORC Block set program parsing..." << std::flush;
        const int ret = orc_parse_full(source.c_str(), &programs, &log);
        std::cerr << " done - " << ret << std::endl;

        //handle the logging result
        const std::string log_string(log?log:"");
        if (log != NULL) free(log);
        if (not log_string.empty()) throw std::runtime_error(log_string);

        //locate the kernel in the list via name
        _program = NULL;
        for (size_t i = 0; i < ret; i++)
        {
            if (name == programs[i]->name) _program = programs[i];
            else free(programs[i]);
        }
        if (_program == NULL)
        {
            throw std::runtime_error("could not find kernel in source: " + name);
        }
    }

    void work(const InputItems &ins, const OutputItems &outs)
    {
        
    }

    OrcBlockParams _params;
    OrcProgram *_program;
};

/***********************************************************************
 * Block factor function
 **********************************************************************/
OrcBlock::sptr OrcBlock::make(void)
{
    return boost::make_shared<OrcBlockImpl>();
}

#else //HAVE_ORC

OrcBlock::sptr OrcBlock::make(void)
{
    throw std::runtime_error("OrcBlock::make sorry, built without ORC support");
}

#endif //HAVE_ORC
