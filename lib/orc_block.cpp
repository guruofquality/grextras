// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#include <grextras/orc_block.hpp>
#include <boost/make_shared.hpp>
#include <stdexcept>
#include <iostream>
#include <boost/foreach.hpp>

using namespace grextras;

OrcBlockParams::OrcBlockParams(void)
{
    kernel_factor = 1.0;
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
    OrcBlockImpl(void):
        gras::Block("GrExtras OrcBlock")
    {
        //NULL
    }

    ~OrcBlockImpl(void)
    {
        //NULL
    }

    OrcBlockParams &params(void)
    {
        return _params;
    }

    void set_program(const std::string &name, const std::string &source);
    void work(const InputItems &ins, const OutputItems &outs);
    void notify_topology(const size_t num_inputs, const size_t num_outputs);
    void propagate_tags(const size_t i, const gras::TagIter &iter);

    size_t _num_outs;
    OrcBlockParams _params;
    OrcExecutor _orc_ex;
    boost::shared_ptr<OrcExecutor> _orc_executor;
    boost::shared_ptr<OrcProgram> _orc_program;
};

/***********************************************************************
 * Scheduler work hooks
 **********************************************************************/
void OrcBlockImpl::notify_topology(const size_t num_inputs, const size_t num_outputs)
{
    if (num_inputs > 4) throw std::runtime_error("ORC Block max inputs 4");
    if (num_outputs > 4) throw std::runtime_error("ORC Block max outputs 4");
    _num_outs = num_outputs;
}

void OrcBlockImpl::propagate_tags(const size_t i, const gras::TagIter &iter)
{
    BOOST_FOREACH(const gras::Tag &t, iter)
    {
        //forward input tags to all outputs - scale the offset given _params
        for (size_t o = 0; o < _num_outs; o++)
        {
            gras::Tag t_o = t;
            t_o.offset -= this->get_consumed(i);
            t_o.offset *= _params.production_factor;
            t_o.offset += this->get_produced(o);
            this->post_output_tag(o, t_o);
        }
    }
}

void OrcBlockImpl::set_program(
    const std::string &name,
    const std::string &source
)
{
    ////////////////////////////////////////////////////////////////////
    // always orc init before using orc
    ////////////////////////////////////////////////////////////////////
    orc_init();

    ////////////////////////////////////////////////////////////////////
    // call into the parser
    ////////////////////////////////////////////////////////////////////
    OrcProgram **programs = NULL;
    char *log = NULL;
    std::cerr << "ORC Block parsing code... " << std::flush;
    const int nkerns = orc_parse_full(source.c_str(), &programs, &log);
    std::cerr << nkerns << " kernels." << std::endl;

    ////////////////////////////////////////////////////////////////////
    // load programs into managed memory asap
    ////////////////////////////////////////////////////////////////////
    std::vector<boost::shared_ptr<OrcProgram> > managed_programs(nkerns);
    for (int i = 0; i < nkerns; i++)
    {
        managed_programs[i].reset(programs[i], orc_program_free);
    }
    free(programs);

    ////////////////////////////////////////////////////////////////////
    // handle the logging result
    ////////////////////////////////////////////////////////////////////
    const std::string log_string(log?log:"");
    if (log != NULL) free(log);
    if (log[0] != '\0')
    {
        throw std::runtime_error(log_string);
    }

    ////////////////////////////////////////////////////////////////////
    // locate the kernel in the list via name
    ////////////////////////////////////////////////////////////////////
    std::cerr << "ORC Block locate kernel... " << std::flush;
    for (size_t i = 0; i < managed_programs.size(); i++)
    {
        if (name == managed_programs[i]->name)
        {
            _orc_program = managed_programs[i];
        }
    }
    if (not _orc_program)
    {
        throw std::runtime_error("could not find kernel in source: " + name);
    }
    std::cerr << "found." << std::endl;

    ////////////////////////////////////////////////////////////////////
    // compile the program
    ////////////////////////////////////////////////////////////////////
    std::cerr << "ORC Block compile code... " << std::flush;
    const OrcCompileResult result = orc_program_compile(_orc_program.get());
    if (not ORC_COMPILE_RESULT_IS_SUCCESSFUL(result))
    {
        throw std::runtime_error("failed to compile program for kernel: " + name);
    }
    std::cerr << "complete." << std::endl;

    ////////////////////////////////////////////////////////////////////
    // create execution unit
    ////////////////////////////////////////////////////////////////////
    OrcExecutor *e = orc_executor_new(_orc_program.get());
    _orc_executor.reset(e, orc_executor_free);
}

void OrcBlockImpl::work(const InputItems &ins, const OutputItems &outs)
{
    //calculate production/consumption params
    size_t num_input_items, num_output_items;
    if (_params.production_factor > 1.0)
    {
        num_output_items = std::min(size_t(ins.min()*_params.production_factor), outs.min());
        num_input_items = size_t(num_output_items/_params.production_factor);
    }
    else
    {
        num_input_items = std::min(size_t(outs.min()/_params.production_factor), ins.min());
        num_output_items = size_t(num_input_items*_params.production_factor);
    }

    //load the executor with source and dest buffers
    size_t input_index = 0, output_index = 0;
    for (size_t i = 0; i < ORC_N_VARIABLES; i++)
    {
        if (_orc_program->vars[i].vartype == ORC_VAR_TYPE_DEST) 
        {
            void *dst = outs[output_index++].cast<void *>();
            orc_executor_set_array(_orc_executor.get(), i, dst);
        }
        if (_orc_program->vars[i].vartype == ORC_VAR_TYPE_SRC)
        {
            void *src = const_cast<void *>(ins[input_index++].cast<const void *>());
            orc_executor_set_array(_orc_executor.get(), i, src);
        }
    }

    //execute the orc code;
    orc_executor_set_n(_orc_executor.get(), size_t(_params.kernel_factor*num_input_items));
    orc_executor_run(_orc_executor.get());

    //produce consume fixed
    this->consume(num_input_items-_params.consumption_offset);
    this->produce(num_output_items);
}

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
