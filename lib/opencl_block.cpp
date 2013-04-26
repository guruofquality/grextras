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

//boost::mutex _shared_buffer_table_write_mutex;
struct BufferTableEntry
{
    
    cl::Buffer cl_buffer;
};
static std::vector<BufferTableEntry> shared_buffer_table;


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
    void set_host_access_mode(const std::string &direction, const std::vector<std::string> &modes);
    void set_device_access_mode(const std::string &direction, const std::vector<std::string> &modes);
    gras::BufferQueueSptr output_buffer_allocator(const size_t which_output, const gras::SBufferConfig &config);
    gras::BufferQueueSptr input_buffer_allocator(const size_t which_input, const gras::SBufferConfig &config);

    std::vector<std::string> _input_host_modes;
    std::vector<std::string> _output_host_modes;
    std::vector<std::string> _input_device_modes;
    std::vector<std::string> _output_device_modes;

    cl_device_type _cl_dev_type;
    cl::Context _cl_context;
    std::vector<cl::Platform> _cl_platforms;
    cl::Platform _cl_platform;
    std::vector<cl::Device> _cl_devices;
    cl::Device _cl_device;
    cl::Program _cl_program;
    cl::Kernel _cl_kernel;
    cl::CommandQueue _cl_cmd_queue;

    //temp work containers for unmap
    std::vector<const cl::Buffer *> _work_input_buffs;
    std::vector<const cl::Buffer *> _work_output_buffs;
    std::vector<void *> _work_input_ptrs;
    std::vector<void *> _work_output_ptrs;
    cl::Buffer _work_input_sizes_buffer;
    cl::Buffer _work_output_sizes_buffer;
    cl_uint *_work_input_sizes;
    cl_uint *_work_output_sizes;
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
    cl_int err = CL_SUCCESS;
    _work_input_buffs.resize(num_inputs);
    _work_input_ptrs.resize(num_inputs);
    _work_input_sizes_buffer = cl::Buffer(
        _cl_context,
        CL_MEM_READ_WRITE | CL_MEM_ALLOC_HOST_PTR,
        num_inputs * sizeof(cl_uint),
        NULL, &err
    );
    checkErr(err, "_work_input_sizes_buffer alloc");
    err = _work_input_sizes_buffer.getInfo(CL_MEM_HOST_PTR, &_work_input_sizes);
    checkErr(err, "_work_input_sizes_buffer.getInfo(CL_MEM_HOST_PTR)");

    _work_output_buffs.resize(num_outputs);
    _work_output_ptrs.resize(num_outputs);
    _work_output_sizes_buffer = cl::Buffer(
        _cl_context,
        CL_MEM_READ_WRITE | CL_MEM_ALLOC_HOST_PTR,
        num_outputs * sizeof(cl_uint),
        NULL, &err
    );
    checkErr(err, "_work_output_sizes_buffer alloc");
    err = _work_output_sizes_buffer.getInfo(CL_MEM_HOST_PTR, &_work_output_sizes);
    checkErr(err, "_work_output_sizes_buffer.getInfo(CL_MEM_HOST_PTR)");
}

void OpenClBlockImpl::work(const InputItems &ins, const OutputItems &outs)
{
    cl_int err = CL_SUCCESS;
    size_t arg_index = 0;

    //map input buffers
    for (size_t i = 0; i < ins.size(); i++)
    {
        gras::SBuffer buffer = get_input_buffer(i);
        _work_input_buffs[i] = reinterpret_cast<const cl::Buffer *>(buffer.get_user_index());
        _work_input_ptrs[i] = _cl_cmd_queue.enqueueMapBuffer(
            *_work_input_buffs[i], //buffer
            CL_TRUE,// blocking_map
            CL_MEM_READ_ONLY, //cl_map_map_flags
            buffer.offset, //offset
            buffer.length //size
        );
        _work_input_sizes[i] = ins[i].size();
        _cl_kernel.setArg(arg_index++, _work_input_ptrs[i]);
    }

    //map output buffers
    for (size_t i = 0; i < outs.size(); i++)
    {
        gras::SBuffer buffer = get_output_buffer(i);
        _work_output_buffs[i] = reinterpret_cast<const cl::Buffer *>(buffer.get_user_index());
        _work_output_ptrs[i] = _cl_cmd_queue.enqueueMapBuffer(
            *_work_output_buffs[i], //buffer
            CL_TRUE,// blocking_map
            CL_MEM_WRITE_ONLY, //cl_map_map_flags
            buffer.offset, //offset
            buffer.length //size
        );
        _work_output_sizes[i] = outs[i].size();
        _cl_kernel.setArg(arg_index++, _work_output_ptrs[i]);
    }

    //set kernel args for consume and produce item counts
    _cl_kernel.setArg(arg_index++, _work_input_sizes_buffer);
    _cl_kernel.setArg(arg_index++, _work_output_sizes_buffer);

    //enqueue work
    err = _cl_cmd_queue.enqueueNDRangeKernel(
        _cl_kernel, //kernel
        cl::NullRange, //offset
        cl::NDRange(std::max(ins.max(), outs.max()))/*TODO*/, //global
        cl::NDRange(1)/*TODO*/ //local
    );
    checkErr(err, "enqueueNDRangeKernel");

    //read back consume and produce results
    err = _cl_cmd_queue.enqueueReadBuffer(
        _work_input_sizes_buffer, //buffer
        CL_TRUE, //blocking_read
        0, //offset
        ins.size()*sizeof(cl_uint), //size
        _work_input_sizes //pointer
    );
    checkErr(err, "_work_input_sizes_buffer enqueueReadBuffer");
    err = _cl_cmd_queue.enqueueReadBuffer(
        _work_output_sizes_buffer, //buffer
        CL_TRUE, //blocking_read
        0, //offset
        outs.size()*sizeof(cl_uint), //size
        _work_output_sizes //pointer
    );
    checkErr(err, "_work_output_sizes_buffer enqueueReadBuffer");

    //wait for work to complete
    err = _cl_cmd_queue.finish();
    checkErr(err, "wait work finish");

    //unmap buffers
    for (size_t i = 0; i < ins.size(); i++)
    {
        this->consume(i, _work_input_sizes[i]);
        _cl_cmd_queue.enqueueUnmapMemObject(*_work_input_buffs[i], _work_input_ptrs[i]);
    }
    for (size_t i = 0; i < outs.size(); i++)
    {
        this->produce(i, _work_output_sizes[i]);
        _cl_cmd_queue.enqueueUnmapMemObject(*_work_output_buffs[i], _work_output_ptrs[i]);
    }

    //wait for unmap to complete
    err = _cl_cmd_queue.finish();
    checkErr(err, "wait unmap finish");
}

/***********************************************************************
 * Buffering and access modes
 **********************************************************************/
void OpenClBlockImpl::set_host_access_mode(const std::string &direction, const std::vector<std::string> &modes)
{
    if (direction == "INPUT") _input_host_modes = modes;
    else if (direction == "OUTPUT") _output_host_modes = modes;
    else throw std::runtime_error("set_host_access_mode unknown direction " + direction);
}

void OpenClBlockImpl::set_device_access_mode(const std::string &direction, const std::vector<std::string> &modes)
{
    if (direction == "INPUT") _input_device_modes = modes;
    else if (direction == "OUTPUT") _output_device_modes = modes;
    else throw std::runtime_error("set_device_access_mode unknown direction " + direction);
}

static cl_mem_flags mode_str_to_flags(const std::string &hmode, const std::string &dmode)
{
    cl_mem_flags flags = 0;

    if (hmode == "RW") flags |= CL_MEM_ALLOC_HOST_PTR;
    else if (hmode == "RO") flags |= CL_MEM_ALLOC_HOST_PTR | CL_MEM_HOST_READ_ONLY;
    else if (hmode == "WO") flags |= CL_MEM_ALLOC_HOST_PTR | CL_MEM_HOST_WRITE_ONLY;
    else if (hmode == "XX") flags |= CL_MEM_HOST_NO_ACCESS;
    else throw std::runtime_error("opencl block unknown host mode: " + hmode);

    if (dmode == "RW") flags |= CL_MEM_READ_WRITE;
    else if (dmode == "RO") flags |= CL_MEM_READ_ONLY;
    else if (dmode == "WO") flags |= CL_MEM_WRITE_ONLY;
    else throw std::runtime_error("opencl block unknown device mode: " + dmode);

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
        my_vec_get(_output_host_modes, which_output, "RO"),
        my_vec_get(_output_device_modes, which_output, "WO"));
    return gras::BufferQueueSptr(
        new OpenClBufferQueue(config, 2, _cl_context, flags));
}

gras::BufferQueueSptr OpenClBlockImpl::input_buffer_allocator(
    const size_t which_input, const gras::SBufferConfig &config
){
    const cl_mem_flags flags = mode_str_to_flags(
        my_vec_get(_input_host_modes, which_input, "WO"),
        my_vec_get(_input_device_modes, which_input, "RO"));
    return gras::BufferQueueSptr(
        new OpenClBufferQueue(config, 2, _cl_context, flags));
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
