// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#include <grextras/opencl_block.hpp>
#include <stdexcept>
#include <boost/format.hpp>
#include <iostream>
#include <utility>
#include <vector>
#include <boost/make_shared.hpp>

//http://www.cc.gatech.edu/~vetter/keeneland/tutorial-2011-04-14/06-intro_to_opencl.pdf
//http://www.codeproject.com/Articles/92788/Introductory-Tutorial-to-OpenCL
//http://developer.amd.com/tools/heterogeneous-computing/amd-accelerated-parallel-processing-app-sdk/introductory-tutorial-to-opencl/
//http://www.khronos.org/registry/cl/specs/opencl-cplusplus-1.1.pdf
//http://streamcomputing.eu/blog/2013-02-03/opencl-basics-flags-for-the-creating-memory-objects/

using namespace grextras;

#ifdef HAVE_OPENCL

#define MY_HERE() std::cerr << __FILE__ << ":" << __LINE__ << std::endl;

static const size_t OPENCL_BLOCK_NUM_BUFFS = 2;

#include <CL/cl.hpp>

static GRAS_FORCE_INLINE void checkErr(cl_int err, const char * name)
{
    if GRAS_UNLIKELY(err != CL_SUCCESS) throw std::runtime_error(str(
        boost::format("ERROR: %s (%d)") % name % err));
}

/***********************************************************************
 * buffer implementation
 **********************************************************************/
#include "opencl_buffer.hpp"

/***********************************************************************
 * impl class definition
 **********************************************************************/
struct OpenClBlockImpl : OpenClBlock
{
    OpenClBlockImpl(const std::string &dev_type);
    ~OpenClBlockImpl(void);
    void set_program(const std::string &name, const std::string &source, const std::string &options);
    void notify_topology(const size_t num_inputs, const size_t num_outputs);
    void work(const InputItems &ins, const OutputItems &outs);
    void set_access_mode(const std::string &direction, const std::vector<std::string> &modes);
    gras::BufferQueueSptr output_buffer_allocator(const size_t which_output, const gras::SBufferConfig &config);
    gras::BufferQueueSptr input_buffer_allocator(const size_t which_input, const gras::SBufferConfig &config);

    std::vector<std::string> _input_access_modes;
    std::vector<std::string> _output_access_modes;

    cl_device_type _cl_dev_type;
    cl::Context _cl_context;
    std::vector<cl::Platform> _cl_platforms;
    cl::Platform _cl_platform;
    std::vector<cl::Device> _cl_devices;
    cl::Device _cl_device;
    cl::Program _cl_program;
    cl::Kernel _cl_kernel;
    cl::CommandQueue _cl_cmd_queue;

    //temp work containers for map/unmap
    std::vector<clBufferSptr> _work_input_buffs;
    std::vector<clBufferSptr> _work_output_buffs;
};

/***********************************************************************
 * Block constructor
 **********************************************************************/
OpenClBlockImpl::OpenClBlockImpl(const std::string &dev_type):
    gras::Block("GrExtras OpenClBlock")
{

    /***************************************************************
     * Determine device type
     **************************************************************/
    if (dev_type == "CPU") _cl_dev_type = CL_DEVICE_TYPE_CPU;
    else if (dev_type == "GPU") _cl_dev_type = CL_DEVICE_TYPE_GPU;
    else throw std::runtime_error("OpenClBlock unknown device type: " + dev_type);

    /***************************************************************
     * Enumerate platforms
     **************************************************************/
    {
        cl::Platform::get(&_cl_platforms);
        checkErr(_cl_platforms.size() != 0 ? CL_SUCCESS : -1, "cl::Platform::get");
        std::cerr << "Number of platforms: " << _cl_platforms.size() << ", "
                  << "selecting 0 ..." << std::endl;
        _cl_platform = _cl_platforms[0];
        std::string platformStr;
        _cl_platform.getInfo((cl_platform_info)CL_PLATFORM_NAME, &platformStr);
        std::cerr << "    name: " << platformStr << std::endl;
        _cl_platform.getInfo((cl_platform_info)CL_PLATFORM_VENDOR, &platformStr);
        std::cerr << "    vendor: " << platformStr << std::endl;
        _cl_platform.getInfo((cl_platform_info)CL_PLATFORM_VERSION, &platformStr);
        std::cerr << "    version: " << platformStr << std::endl;
    }

    /***************************************************************
     * create context
     **************************************************************/
    {
        cl_context_properties cprops[3] =
            {CL_CONTEXT_PLATFORM, (cl_context_properties)(_cl_platform)(), 0};

        cl_int err = CL_SUCCESS;
        _cl_context = cl::Context(
            _cl_dev_type,
            cprops,
            NULL,
            NULL,
            &err
        );
        checkErr(err, "cl::Context");
    }

    /***************************************************************
     * enumerate devices
     **************************************************************/
    {
        _cl_devices = _cl_context.getInfo<CL_CONTEXT_DEVICES>();
        checkErr(_cl_devices.size() > 0 ? CL_SUCCESS : -1, "devices.size() > 0");
        std::cerr << "Number of devices: " << _cl_devices.size() << ", "
                  << "selecting 0 ..." << std::endl;
        _cl_device = _cl_devices[0];
    }

    /***************************************************************
     * command queue
     **************************************************************/
    {
        cl_int err = CL_SUCCESS;
        _cl_cmd_queue = cl::CommandQueue(_cl_context, _cl_device, 0, &err);
        checkErr(err, "cl::CommandQueue");
    }
}

OpenClBlockImpl::~OpenClBlockImpl(void)
{
    //NOP
}

/***********************************************************************
 * Program creation + compilation
 **********************************************************************/
void OpenClBlockImpl::set_program(const std::string &name, const std::string &source, const std::string &options)
{
    cl::Program::Sources cl_source(1,
        std::make_pair(source.c_str(), source.length()+1));
    _cl_program = cl::Program(_cl_context, cl_source);
    cl_int err = _cl_program.build(_cl_devices, options.c_str());
    checkErr(err, "Program::build");

    _cl_kernel = cl::Kernel(_cl_program, name.c_str(), &err);
    checkErr(err, "Kernel create");
}

/***********************************************************************
 * Scheduler work hooks
 **********************************************************************/
void OpenClBlockImpl::notify_topology(const size_t num_inputs, const size_t num_outputs)
{
    _work_input_buffs.resize(num_inputs);
    _work_output_buffs.resize(num_outputs);
}

void OpenClBlockImpl::work(const InputItems &ins, const OutputItems &outs)
{
    cl_int err = CL_SUCCESS;
    size_t arg_index = 0;

        MY_HERE();
    //TODO input buffers could be from another queue - needs conditional check and handle

    //pre-work input buffers
    MY_HERE();
    for (size_t i = 0; i < ins.size(); i++)
    {
        gras::SBuffer buffer = get_input_buffer(i);
        _work_input_buffs[i] = get_opencl_buffer(buffer);
        _cl_cmd_queue.enqueueUnmapMemObject(*_work_input_buffs[i], buffer.get_actual_memory());
        _cl_kernel.setArg(arg_index++, *_work_input_buffs[i]);
    }

    //pre-work output buffers
    MY_HERE();
    for (size_t i = 0; i < outs.size(); i++)
    {
        gras::SBuffer buffer = get_output_buffer(i);
        _work_output_buffs[i] = get_opencl_buffer(buffer);
        _cl_kernel.setArg(arg_index++, *_work_output_buffs[i]);
    }

    const cl_uint num = std::min(ins.min(), outs.min());

    //enqueue work
    err = _cl_cmd_queue.enqueueNDRangeKernel(
        _cl_kernel, //kernel
        cl::NullRange, //offset
        cl::NDRange(num)/*TODO*/, //global
        cl::NDRange(1)/*TODO*/ //local
    );
    checkErr(err, "enqueueNDRangeKernel");

    //wait for unmap to complete
    err = _cl_cmd_queue.finish();
    checkErr(err, "wait work finish");

    //post-work output buffers
    MY_HERE();
    for (size_t i = 0; i < ins.size(); i++)
    {
        _work_input_buffs[i].reset();
    }

    //post-work output buffers
    MY_HERE();
    for (size_t i = 0; i < outs.size(); i++)
    {
        gras::SBuffer buffer = get_output_buffer(i);
        buffer->config.memory = _cl_cmd_queue.enqueueMapBuffer(
            *_work_output_buffs[i], //buffer
            CL_TRUE, // blocking_map
            CL_MAP_READ, //cl_map_map_flags
            0, //offset
            buffer.get_actual_length() //size
        );
        _work_output_buffs[i].reset();
    }

    //produce consume fixed
    this->consume(0, num);
    this->produce(0, num);
}

/***********************************************************************
 * Buffering and access modes
 **********************************************************************/
void OpenClBlockImpl::set_access_mode(const std::string &direction, const std::vector<std::string> &modes)
{
    if (direction == "INPUT") _input_access_modes = modes;
    else if (direction == "OUTPUT") _output_access_modes = modes;
    else throw std::runtime_error("set_access_mode unknown direction " + direction);
}

static cl_mem_flags mode_str_to_flags(
    const std::string &direction, const size_t which, const std::string &mode
)
{
    std::cout << boost::format("Making %s buffers for %s port %u...\n") % mode % direction % which;

    cl_mem_flags flags = CL_MEM_ALLOC_HOST_PTR;
    if      (mode == "RW") flags |= CL_MEM_READ_WRITE;
    else if (mode == "RO") flags |= CL_MEM_READ_ONLY;
    else if (mode == "WO") flags |= CL_MEM_WRITE_ONLY;
    else throw std::runtime_error("opencl block unknown device mode: " + mode);

    return flags;
}

static std::string my_vec_get(const std::vector<std::string> &v, const size_t index, const std::string &def)
{
    if (v.empty()) return def;
    if (index < v.size()) return v[index];
    return v.back();
}

gras::BufferQueueSptr OpenClBlockImpl::output_buffer_allocator(
    const size_t which_output, const gras::SBufferConfig &config
){
    const cl_mem_flags flags = mode_str_to_flags(
        "output", which_output, my_vec_get(_output_access_modes, which_output, "WO"));
    return gras::BufferQueueSptr(new OpenClBufferQueue(
        config, OPENCL_BLOCK_NUM_BUFFS, _cl_context, _cl_cmd_queue, flags, OPENCL_BUFFER_PUSH_UNMAP));
}

gras::BufferQueueSptr OpenClBlockImpl::input_buffer_allocator(
    const size_t which_input, const gras::SBufferConfig &config
){
    const cl_mem_flags flags = mode_str_to_flags(
        "input", which_input, my_vec_get(_input_access_modes, which_input, "RO"));
    return gras::BufferQueueSptr(new OpenClBufferQueue(
        config, OPENCL_BLOCK_NUM_BUFFS, _cl_context, _cl_cmd_queue, flags, OPENCL_BUFFER_PUSH_MAP));
}

/***********************************************************************
 * Block factor function
 **********************************************************************/
OpenClBlock::sptr OpenClBlock::make(const std::string &dev_type)
{
    return boost::make_shared<OpenClBlockImpl>(dev_type);
}

#else //HAVE_OPENCL

OpenClBlock::sptr OpenClBlock::make(const std::string &)
{
    throw std::runtime_error("OpenClBlock::make sorry, built without Open CL support");
}

#endif //HAVE_OPENCL
