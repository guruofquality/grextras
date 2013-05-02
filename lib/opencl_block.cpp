// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#include <grextras/opencl_block.hpp>
#include <stdexcept>
#include <boost/format.hpp>
#include <iostream>
#include <utility>
#include <vector>
#include <boost/make_shared.hpp>

using namespace grextras;

OpenClBlockParams::OpenClBlockParams(void)
{
    global_factor = 1.0;
    local_size = 1;
    production_factor = 1.0;
    consumption_offset = 0;
}

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

    OpenClBlockParams &params(void){return _params;}
    OpenClBlockParams _params;

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

    std::vector<clBufferSptr> _temp_input_buffs;
    std::vector<clBufferSptr> _temp_output_buffs;

    size_t _extra_cl_buffer_allocs;
};

/***********************************************************************
 * Block constructor
 **********************************************************************/
OpenClBlockImpl::OpenClBlockImpl(const std::string &dev_type):
    gras::Block("GrExtras OpenClBlock"),
    _extra_cl_buffer_allocs(0)
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
    _temp_input_buffs.clear();
    _temp_output_buffs.clear();
    if (_extra_cl_buffer_allocs != 0)
    {
        std::cerr << "~OpenClBlock extra buffer allocs: " << _extra_cl_buffer_allocs << std::endl;
    }
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
    _temp_input_buffs.resize(num_inputs);
    _temp_output_buffs.resize(num_outputs);
}

void OpenClBlockImpl::work(const InputItems &ins, const OutputItems &outs)
{
    cl_int err = CL_SUCCESS;
    size_t arg_index = 0;

    //pre-work input buffers
    for (size_t i = 0; i < ins.size(); i++)
    {
        if (ins[i].size() <= _params.consumption_offset)
        {
            this->mark_input_fail(i);
            return;
        }
        gras::SBuffer buffer = get_input_buffer(i);
        clBufferSptr cl_buff = get_opencl_buffer(buffer);
        if GRAS_UNLIKELY(not cl_buff) //not our cl buffer, just copy into a new one so things work
        {
            const cl_mem_flags flags = CL_MEM_COPY_HOST_PTR | CL_MEM_READ_WRITE;
            _temp_input_buffs[i].reset(new cl::Buffer(_cl_context, flags, buffer.length, buffer.get(), &err));
            checkErr(err, "tmp input buffer - cl::Buffer");
            _extra_cl_buffer_allocs++;
            cl_buff = _temp_input_buffs[i];
        }
        _cl_kernel.setArg(arg_index++, *cl_buff);
    }

    //pre-work output buffers
    for (size_t i = 0; i < outs.size(); i++)
    {
        gras::SBuffer buffer = get_output_buffer(i);
        this->pop_output_buffer(i, 0); //clear auto popping - we use produce
        clBufferSptr cl_buff = get_opencl_buffer(buffer);
        if GRAS_UNLIKELY(not cl_buff) throw std::runtime_error("output was not a cl buffer");
        _cl_kernel.setArg(arg_index++, *cl_buff);
    }

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

    //calculate kernel execution params
    const size_t global = size_t(_params.global_factor*num_input_items);
    const size_t local = _params.local_size;

    //enqueue work
    err = _cl_cmd_queue.enqueueNDRangeKernel(
        _cl_kernel, //kernel
        cl::NullRange, //offset
        cl::NDRange(global), //global
        cl::NDRange(local) //local
    );
    checkErr(err, "enqueueNDRangeKernel");

    //wait on kernel
    err = _cl_cmd_queue.finish();
    checkErr(err, "cmd queue kernel finish");

    //produce consume fixed
    this->consume(num_input_items-_params.consumption_offset);
    this->produce(num_output_items);
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
    const std::string access_mode = my_vec_get(_output_access_modes, which_output, "WO");
    const cl_mem_flags flags = mode_str_to_flags("output", which_output, access_mode);
    return gras::BufferQueueSptr(new OpenClBufferQueue(
        config, OPENCL_BLOCK_NUM_BUFFS, _cl_context, _cl_cmd_queue, flags, CL_MAP_READ));
}

gras::BufferQueueSptr OpenClBlockImpl::input_buffer_allocator(
    const size_t which_input, const gras::SBufferConfig &config
){
    const std::string access_mode = my_vec_get(_input_access_modes, which_input, "RO");
    const cl_mem_flags flags = mode_str_to_flags("input", which_input, access_mode);
    return gras::BufferQueueSptr(new OpenClBufferQueue(
        config, OPENCL_BLOCK_NUM_BUFFS, _cl_context, _cl_cmd_queue, flags, CL_MAP_WRITE));
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
