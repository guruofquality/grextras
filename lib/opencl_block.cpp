// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#include <grextras/opencl_block.hpp>
#include <stdexcept>
#include <boost/format.hpp>
#include <iostream>
#include <utility>
#include <vector>
#include <boost/make_shared.hpp>

//http://www.codeproject.com/Articles/92788/Introductory-Tutorial-to-OpenCL
//http://developer.amd.com/tools/heterogeneous-computing/amd-accelerated-parallel-processing-app-sdk/introductory-tutorial-to-opencl/
//http://www.khronos.org/registry/cl/specs/opencl-cplusplus-1.1.pdf

using namespace grextras;

#ifdef HAVE_OPENCL

#include <CL/cl.hpp>

/***********************************************************************
 * Helper functions
 **********************************************************************/
inline void checkErr(cl_int err, const char * name)
{
    if (err != CL_SUCCESS) throw std::runtime_error(str(
        boost::format("ERROR: %s (%d)") % name % err));
}

static std::string my_vec_get(const std::vector<std::string> &v, const size_t index, const std::string &def)
{
    if (v.empty()) return def;
    if (index < v.size()) return v[index];
    return v.back();
}

static int mode_str_to_flags(const std::string &hmode, const std::string &dmode)
{
    int flags = 0;

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

/***********************************************************************
 * shared buffer table
 **********************************************************************/
//boost::mutex _shared_buffer_table_write_mutex;
struct BufferTableEntry
{
    
    cl::Buffer cl_buffer;
};
static std::vector<BufferTableEntry> shared_buffer_table;


/***********************************************************************
 * Block implementation
 **********************************************************************/
struct OpenClBlockImpl : OpenClBlock
{
    OpenClBlockImpl(const std::string &dev_type):
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

    ~OpenClBlockImpl(void)
    {
        //NOP
    }

    void set_program(const std::string &name, const std::string &source, const std::string &options)
    {
        cl::Program::Sources cl_source(1,
            std::make_pair(source.c_str(), source.length()+1));
        _cl_program = cl::Program(_cl_context, cl_source);
        cl_int err = _cl_program.build(_cl_devices, options.c_str());
        checkErr(err, "Program::build");

        _cl_kernel = cl::Kernel(_cl_program, name.c_str(), &err);
        checkErr(err, "Kernel create");
    }

    void notify_topology(const size_t num_inputs, const size_t num_outputs)
    {
        
    }

    void work(const InputItems &ins, const OutputItems &outs)
    {
        //http://streamcomputing.eu/blog/2013-02-03/opencl-basics-flags-for-the-creating-memory-objects/
        //inputs
        /*
        cl_mem input = clCreateBuffer(context, CL_MEM_READ_ONLY | 
            CL_MEM_USE_HOST_PTR, sizeof(float) * count, &data_in, NULL);
        void* data_in_ptr = clEnqueueMapBuffer(queue, input, CL_TRUE, 
            CL_MAP_READ, 0, sizeof(float) * count, 0, NULL, NULL, &err);
        err = clSetKernelArg(kernel, 0, sizeof(cl_mem), &input)
        clEnqueueUnmapMemObject(queue, input, data_in_ptr, 0, NULL, NULL);
        err = clReleaseMemObject(input);
        */
        //outputs
        /*
        cl_mem output = clCreateBuffer(context, CL_MEM_WRITE_ONLY | 
            CL_MEM_USE_HOST_PTR, sizeof(float) * count, &data_out, NULL);
        void* data_out_ptr = clEnqueueMapBuffer(queue, output, CL_TRUE, 
            CL_MAP_WRITE, 0, sizeof(float) * count, 0, NULL, NULL, &err);
        err = clSetKernelArg(kernel, 0, sizeof(cl_mem), &output)
        clEnqueueUnmapMemObject(queue, output, data_out_ptr, 0, NULL, NULL); 
        err = clReleaseMemObject(output);
        */
    }

    void set_host_access_mode(const std::string &direction, const std::vector<std::string> &modes)
    {
        if (direction == "INPUT") _input_host_modes = modes;
        else if (direction == "OUTPUT") _output_host_modes = modes;
        else throw std::runtime_error("set_host_access_mode unknown direction " + direction);
    }

    void set_device_access_mode(const std::string &direction, const std::vector<std::string> &modes)
    {
        if (direction == "INPUT") _input_device_modes = modes;
        else if (direction == "OUTPUT") _output_device_modes = modes;
        else throw std::runtime_error("set_device_access_mode unknown direction " + direction);
    }

    gras::BufferQueueSptr output_buffer_allocator(
        const size_t which_output,
        const gras::SBufferConfig &config
    ){
        const int flags = mode_str_to_flags(
            my_vec_get(_output_host_modes, which_output, "RO"),
            my_vec_get(_output_device_modes, which_output, "WO")
        );
    }

    gras::BufferQueueSptr input_buffer_allocator(
        const size_t which_input,
        const gras::SBufferConfig &config
    ){
        const int flags = mode_str_to_flags(
            my_vec_get(_input_host_modes, which_input, "WO"),
            my_vec_get(_input_device_modes, which_input, "RO")
        );
    }

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

};

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
