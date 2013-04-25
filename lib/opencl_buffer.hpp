// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#ifndef INCLUDED_GREXTRAS_LIB_OPENCL_BUFFER_HPP
#define INCLUDED_GREXTRAS_LIB_OPENCL_BUFFER_HPP

#include <gras/buffer_queue.hpp>
#include <boost/circular_buffer.hpp>

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

    std::vector<cl::Buffer> _buffers;
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
        cl_int err = CL_SUCCESS;
        cl::Buffer buffer(
            context,
            flags,
            config.length,
            NULL,
            &err
        );
        checkErr(err, "cl::Buffer");
        _buffers.push_back(buffer);
    }
}

OpenClBufferQueue::~OpenClBufferQueue(void)
{
    _token.reset();
    _queue.clear();
}

gras::SBuffer &OpenClBufferQueue::front(void)
{
    
}

void OpenClBufferQueue::pop(void)
{
    
}

void OpenClBufferQueue::push(const gras::SBuffer &buff)
{
    //is it my buffer? otherwise dont keep it
    if GRAS_UNLIKELY(buff->config.token.lock() != _token) return;

    _queue.push_back(buff);
}

bool OpenClBufferQueue::empty(void) const
{
    
}

#endif //INCLUDED_GREXTRAS_LIB_OPENCL_BUFFER_HPP
