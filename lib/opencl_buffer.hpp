// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#ifndef INCLUDED_GREXTRAS_LIB_OPENCL_BUFFER_HPP
#define INCLUDED_GREXTRAS_LIB_OPENCL_BUFFER_HPP

#include <gras/buffer_queue.hpp>
#include <boost/circular_buffer.hpp>
#include <boost/bind.hpp>

static void opencl_buffer_delete(gras::SBuffer &, cl::Buffer *cl_buffer)
{
    delete cl_buffer;
}

struct OpenClBufferQueue : gras::BufferQueue
{
    OpenClBufferQueue(
        const gras::SBufferConfig &config,
        const size_t num_buffs,
        const cl::Context &context,
        const cl_mem_flags flags
    );

    ~OpenClBufferQueue(void);

    //! Get a reference to the buffer at the front of the queue
    gras::SBuffer &front(void);

    //! Pop off the used portion of the queue
    void pop(void);

    //! Push a used buffer back into the queue
    void push(const gras::SBuffer &buff);

    //! Is the queue empty?
    bool empty(void) const;

    gras::SBufferToken _token;
    boost::circular_buffer<gras::SBuffer> _queue;
};

OpenClBufferQueue::OpenClBufferQueue(
    const gras::SBufferConfig &config,
    const size_t num_buffs,
    const cl::Context &context,
    const cl_mem_flags flags
):
    _token(config.token), //save config, its holds token
    _queue(boost::circular_buffer<gras::SBuffer>(num_buffs))
{
    for (size_t i = 0; i < num_buffs; i++)
    {
        //actually bind buffer to sbuffer destructor
        cl_int err = CL_SUCCESS;
        cl::Buffer *cl_buffer = new cl::Buffer(
            context,
            flags,
            config.length,
            NULL,
            &err
        );
        checkErr(err, "cl::Buffer");
        void *host_ptr = NULL;
        err = cl_buffer->getInfo(CL_MEM_HOST_PTR, &host_ptr);
        checkErr(err, "buffer.getInfo(CL_MEM_HOST_PTR)");

        gras::SBufferConfig sconfig = config;
        sconfig.user_index = size_t(cl_buffer); //where to get cl buffer
        sconfig.memory = host_ptr;
        sconfig.deleter = boost::bind(&opencl_buffer_delete, _1, cl_buffer);
        gras::SBuffer buff(sconfig);
        //buffer derefs and returns to this queue thru token callback
    }
}

OpenClBufferQueue::~OpenClBufferQueue(void)
{
    _token.reset();
    _queue.clear();
}

gras::SBuffer &OpenClBufferQueue::front(void)
{
    return _queue.front();
}

void OpenClBufferQueue::pop(void)
{
    _queue.front().reset();
    _queue.pop_front();
}

void OpenClBufferQueue::push(const gras::SBuffer &buff)
{
    //is it my buffer? otherwise dont keep it
    if GRAS_UNLIKELY(buff->config.token.lock() != _token) return;

    _queue.push_back(buff);
}

bool OpenClBufferQueue::empty(void) const
{
    return _queue.empty();
}

#endif //INCLUDED_GREXTRAS_LIB_OPENCL_BUFFER_HPP
