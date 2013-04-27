// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#ifndef INCLUDED_GREXTRAS_LIB_OPENCL_BUFFER_HPP
#define INCLUDED_GREXTRAS_LIB_OPENCL_BUFFER_HPP

/***********************************************************************
 * buffer table for sbuffer referencing
 **********************************************************************/
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <boost/thread/mutex.hpp>
#include <vector>

typedef boost::shared_ptr<cl::Buffer> clBufferSptr;
typedef boost::weak_ptr<cl::Buffer> clBufferWeak;

struct OpenClBufferTableEntry
{
    clBufferWeak buffer;
    gras::SBufferTokenWeak token;
};

static std::vector<OpenClBufferTableEntry> opencl_buffer_table;

static size_t store_buffer(const OpenClBufferTableEntry &entry)
{
    static boost::mutex mutex;
    boost::mutex::scoped_lock lock(mutex);

    for (size_t i = 0; i < opencl_buffer_table.size(); i++)
    {
        if (opencl_buffer_table[i].buffer.lock()) continue;
        opencl_buffer_table[i] = entry;
        return i;
    }
    opencl_buffer_table.push_back(entry);
    return opencl_buffer_table.size()-1;
}

static inline clBufferSptr get_opencl_buffer(const gras::SBuffer &buff)
{
    const size_t index = buff.get_user_index();
    if (index >=  opencl_buffer_table.size()) return clBufferSptr();
    const OpenClBufferTableEntry &entry = opencl_buffer_table[index];
    clBufferSptr cl_buffer = entry.buffer.lock();
    if (cl_buffer and entry.token.lock() == buff->config.token.lock()) return cl_buffer;
    return clBufferSptr();
}

/***********************************************************************
 * buffer queue implementation
 **********************************************************************/
#include <gras/buffer_queue.hpp>
#include <boost/circular_buffer.hpp>
#include <boost/bind.hpp>

enum OpenClBufferPushAction
{
    OPENCL_BUFFER_PUSH_NONE,
    OPENCL_BUFFER_PUSH_MAP,
    OPENCL_BUFFER_PUSH_UNMAP,
};

static void opencl_buffer_delete(gras::SBuffer &, clBufferSptr /*holds ref*/)
{
    //NOP
}

struct OpenClBufferQueue : gras::BufferQueue
{
    OpenClBufferQueue(
        const gras::SBufferConfig &config,
        const size_t num_buffs,
        const cl::Context &context,
        const cl::CommandQueue &cmd_queue,
        const cl_mem_flags flags,
        const OpenClBufferPushAction action
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

    const OpenClBufferPushAction _action;
    cl::CommandQueue _cmd_queue;
    gras::SBufferToken _token;
    boost::circular_buffer<gras::SBuffer> _queue;
};

OpenClBufferQueue::OpenClBufferQueue(
    const gras::SBufferConfig &config,
    const size_t num_buffs,
    const cl::Context &context,
    const cl::CommandQueue &cmd_queue,
    const cl_mem_flags flags,
    const OpenClBufferPushAction action
):
    _action(action),
    _cmd_queue(cmd_queue),
    _token(config.token), //save config, its holds token
    _queue(boost::circular_buffer<gras::SBuffer>(num_buffs))
{
    for (size_t i = 0; i < num_buffs; i++)
    {
        OpenClBufferTableEntry entry;
        clBufferSptr cl_buffer;

        //actually bind buffer to sbuffer destructor
        cl_int err = CL_SUCCESS;
        cl_buffer.reset(new cl::Buffer(context, flags, config.length, NULL, &err));
        entry.buffer = cl_buffer;
        entry.token = _token;
        checkErr(err, "OpenClBufferQueue - cl::Buffer");

        gras::SBufferConfig sconfig = config;
        sconfig.user_index = store_buffer(entry);
        sconfig.memory = (void *)(~0); //needs something to avoid malloc
        sconfig.deleter = boost::bind(&opencl_buffer_delete, _1, cl_buffer);
        gras::SBuffer buff(sconfig);
        buff->config.memory = NULL;
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

    //find the buffer from table
    clBufferSptr cl_buff = get_opencl_buffer(buff);

    if (_action == OPENCL_BUFFER_PUSH_MAP)
    {
        buff->config.memory = _cmd_queue.enqueueMapBuffer(
            *cl_buff, //buffer
            CL_TRUE, // blocking_map
            CL_MAP_WRITE, //cl_map_map_flags
            0, //offset
            buff.get_actual_length() //size
        );
    }

    if (_action == OPENCL_BUFFER_PUSH_UNMAP and buff->config.memory)
    {
        _cmd_queue.enqueueUnmapMemObject(*cl_buff, buff->config.memory);
        buff->config.memory = NULL;
    }

    _queue.push_back(buff);
}

bool OpenClBufferQueue::empty(void) const
{
    return _queue.empty();
}

#endif //INCLUDED_GREXTRAS_LIB_OPENCL_BUFFER_HPP
