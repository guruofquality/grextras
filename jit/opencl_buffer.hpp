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

//table mapping buffers to OpenCL buffers
static std::vector<OpenClBufferTableEntry> opencl_buffer_table;

//storing a buffer into the table is a mutex-locked thread-safe operation
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

//get the OpenCL buffer given an SBuffer - thread safe, but no locking required
static GRAS_FORCE_INLINE clBufferSptr get_opencl_buffer(const gras::SBuffer &buff)
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

static void opencl_buffer_delete(gras::SBuffer &buffer, clBufferSptr cl_buffer, const cl::CommandQueue &cmd_queue)
{
    cmd_queue.enqueueUnmapMemObject(*cl_buffer, buffer.get_actual_memory());
}

struct OpenClBufferQueue : gras::BufferQueue
{
    OpenClBufferQueue(
        const gras::SBufferConfig &config,
        const size_t num_buffs,
        const cl::Context &context,
        const cl::CommandQueue &cmd_queue,
        const cl_mem_flags flags,
        const cl_map_flags mflags
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

    const cl_map_flags _mflags;
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
    const cl_map_flags mflags
):
    _mflags(mflags),
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

        //create SBufferConfig for this buffer
        gras::SBufferConfig sconfig = config;
        sconfig.user_index = store_buffer(entry);
        sconfig.memory = _cmd_queue.enqueueMapBuffer(
            *cl_buffer, //buffer
            CL_TRUE, // blocking_map
            mflags, //cl_map_flags
            0, //offset
            sconfig.length //size
        );
        //bind the unmap to reverse this map operation at destruction
        sconfig.deleter = boost::bind(&opencl_buffer_delete, _1, cl_buffer, _cmd_queue);
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
    gras::SBuffer &buff = _queue.front();

    //find the buffer from table
    clBufferSptr cl_buff = get_opencl_buffer(buff);

    //perform non blocking write
    //kernel will be enqueued after this
    if (_mflags == CL_MAP_WRITE)
    {
        const cl_int err = _cmd_queue.enqueueWriteBuffer(
            *cl_buff, CL_FALSE, 0,
            buff.get_actual_length(),
            buff.get_actual_memory()
        );
        checkErr(err, "enqueueWriteBuffer");
    }

    //perform blocking read
    //must block before giving downstream memory
    if (_mflags == CL_MAP_READ)
    {
        const cl_int err = _cmd_queue.enqueueReadBuffer(
            *cl_buff, CL_TRUE, 0,
            buff.get_actual_length(),
            buff.get_actual_memory()
        );
        checkErr(err, "enqueueReadBuffer");
    }

    buff.reset();
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
