// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#include <grextras/opencl_block.hpp>
#include <stdexcept>
#include <boost/format.hpp>
#include <iostream>
#include <utility>
#include <boost/make_shared.hpp>

using namespace grextras;

#ifdef HAVE_OPENCL

#define __NO_STD_VECTOR // Use cl::vector instead of STL version
#include <CL/cl.hpp>

/*
    #include <cstdio>
    #include <cstdlib>
    #include <fstream>
    #include <iostream>
    #include <string>
    #include <iterator>

    inline void
    checkErr(cl_int err, const char * name)
    {
        if (err != CL_SUCCESS) throw std::runtime_error(str(
            boost::format("ERROR: %s (err)") % name % err));
    }

        int
    foobarz(void)
    {
    cl_int err;
    cl::vector< cl::Platform > platformList;
    cl::Platform::get(&platformList);
    checkErr(platformList.size()!=0 ? CL_SUCCESS : -1, "cl::Platform::get");
    std::cerr << "Platform number is: " << platformList.size() << std::endl;std::string platformVendor;
    platformList[0].getInfo((cl_platform_info)CL_PLATFORM_VENDOR, &platformVendor);
    std::cerr << "Platform is by: " << platformVendor << "\n";
    cl_context_properties cprops[3] =
    {CL_CONTEXT_PLATFORM, (cl_context_properties)(platformList[0])(), 0};cl::Context context(
    CL_DEVICE_TYPE_CPU,
    cprops,
    NULL,
    NULL,
    &err);
    checkErr(err, "Conext::Context()"); 
}
* */

#include <stdio.h>
#include <stdlib.h>
 
int fark()
{
    int err;
    cl_uint platforms;
    cl_platform_id platform = NULL;
    char cBuffer[1024];
 
    err = clGetPlatformIDs( 1, &platform, &platforms );
    if (err != CL_SUCCESS)
    {
        printf("Error in OpenCL call!\n");
        return EXIT_FAILURE;
    }
    printf("Number of platforms: %d\n", platforms);
 
    err = clGetPlatformInfo( platform, CL_PLATFORM_NAME, sizeof(cBuffer), cBuffer, NULL );
    if (err != CL_SUCCESS)
    {
        printf("Error in OpenCL call!\n");
        return EXIT_FAILURE;
    }
    printf("CL_PLATFORM_NAME :\t %s\n", cBuffer);
 
    err = clGetPlatformInfo( platform, CL_PLATFORM_VERSION,     sizeof(cBuffer), cBuffer, NULL );
    if (err != CL_SUCCESS)
    {
        printf("Error in OpenCL call!\n");
        return EXIT_FAILURE;
    }
    printf("CL_PLATFORM_VERSION :\t %s\n", cBuffer);
}

inline void checkErr(cl_int err, const char * name)
{
    if (err != CL_SUCCESS) throw std::runtime_error(str(
        boost::format("ERROR: %s (%d)") % name % err));
}

struct OpenClBlockImpl : OpenClBlock
{
    OpenClBlockImpl(const std::string &dev_type):
        gras::Block("GrExtras OpenClBlock")
    {

        fark();
        return;

        /***************************************************************
         * Determine device type
         **************************************************************/
        cl_device_type my_device_type = CL_DEVICE_TYPE_CPU;
        if (dev_type == "CPU") my_device_type = CL_DEVICE_TYPE_CPU;
        else if (dev_type == "GPU") my_device_type = CL_DEVICE_TYPE_GPU;
        else throw std::runtime_error("OpenClBlock unknown device type: " + dev_type);

        /***************************************************************
         * Enumerate platforms
         **************************************************************/

        #define OCLB_ASSERT(call) \
        { \
            const cl_int __err = (call); \
            if (__err != CL_SUCCESS) throw std::runtime_error(str(boost::format("Error %s = %d") % #call % __err)); \
        }

        cl_platform_id my_platform_ids[64];
        cl_uint my_num_platforms = 0;
        OCLB_ASSERT(clGetPlatformIDs(64, my_platform_ids, &my_num_platforms));

        std::cout << "my_num_platforms " << my_num_platforms << std::endl;


        {
                int gpu = 1;
                cl_device_id device_id;             // compute device id 
                cl_int err;

            err = clGetDeviceIDs(NULL, gpu ? CL_DEVICE_TYPE_GPU : CL_DEVICE_TYPE_CPU, 1, &device_id, NULL);

            if (err != CL_SUCCESS)

            {

                std::cout << ("Error: Failed to create a device group!\n");

                throw EXIT_FAILURE;

            }
        }
        
        cl::vector<cl::Platform> platformList;
        cl::Platform::get(&platformList);
        checkErr(platformList.size() != 0 ? CL_SUCCESS : -1, "cl::Platform::get");
        std::cerr << "Platform number is: " << platformList.size() << std::endl;
        std::string platformVendor;
        platformList[0].getInfo((cl_platform_info)CL_PLATFORM_VENDOR, &platformVendor);
        std::cerr << "Platform is by: " << platformVendor << "\n";

        /***************************************************************
         * create context
         **************************************************************/
        cl_context_properties cprops[3] =
            {CL_CONTEXT_PLATFORM, (cl_context_properties)(platformList[0])(), 0};

        cl_int err = CL_SUCCESS;
        _cl_context = cl::Context(
            my_device_type,
            cprops,
            NULL,
            NULL,
            &err
        );
        checkErr(err, "cl::Context"); 
    }

    ~OpenClBlockImpl(void)
    {
        
    }


    void work(const InputItems &ins, const OutputItems &outs)
    {
        
    }

    gras::BufferQueueSptr output_buffer_allocator(
        const size_t which_output,
        const gras::SBufferConfig &config
    ){
        
    }

    gras::BufferQueueSptr input_buffer_allocator(
        const size_t which_input,
        const gras::SBufferConfig &config
    ){
        
    }

    cl::Context _cl_context;

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
