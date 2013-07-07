# Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

import unittest
import gras
import grextras
import numpy
import os
import ctypes

ADD_F32_SOURCE = """
#include <gras/block.hpp>
#include <iostream>

struct MyAddFloat32 : gras::Block
{
    MyAddFloat32(void):
        gras::Block("MyAddFloat32")
    {
        this->input_config(0).item_size = sizeof(float);
        this->output_config(0).item_size = sizeof(float);
    }

    void work(const InputItems &ins, const OutputItems &outs)
    {
        const size_t n_nums = std::min(ins.min(), outs.min());
        float *out = outs[0].cast<float *>();
        const float *in0 = ins[0].cast<const float *>();
        const float *in1 = ins[1].cast<const float *>();

        for (size_t i = 0; i < n_nums; i++)
        {
            out[i] = in0[i] + in1[i];
        }

        this->consume(n_nums);
        this->produce(n_nums);
    }
};

gras::Block *make_add_float32(void)
{
    return new MyAddFloat32();
}
"""

ADD_CONST_F32_SOURCE = """
#include <gras/block.hpp>
#include <iostream>

struct MyAddConstFloat32 : gras::Block
{
    MyAddConstFloat32(void):
        gras::Block("MyAddConstFloat32")
    {
        this->input_config(0).item_size = sizeof(float);
        this->output_config(0).item_size = sizeof(float);
        this->set_value(0); //initial state
        this->register_call("set_value", &MyAddConstFloat32::set_value);
        this->register_call("get_value", &MyAddConstFloat32::get_value);
    }

    void set_value(const float &value)
    {
        _value = value;
    }

    float get_value(void)
    {
        return _value;
    }

    void work(const InputItems &ins, const OutputItems &outs)
    {
        const size_t n_nums = std::min(ins.min(), outs.min());
        float *out = outs[0].cast<float *>();
        const float *in = ins[0].cast<const float *>();

        for (size_t i = 0; i < n_nums; i++)
        {
            out[i] = in[i] + _value;
        }

        this->consume(n_nums);
        this->produce(n_nums);
    }

    float _value;
};

gras::Block *make_add_const_float32(void)
{
    return new MyAddConstFloat32();
}
"""

class test_clang_block(unittest.TestCase):

    def setUp(self):
        self.tb = gras.TopBlock()

    def tearDown(self):
        self.tb = None

    def test_add_float32(self):

        #setup clang block parameters
        params = grextras.ClangBlockParams()
        params.name = "make_add_float32"
        params.source = ADD_F32_SOURCE
        params.flags.append('-O3')

        #setup includes (set by test env var)
        for include_dir in os.environ['CLANG_BLOCK_INCLUDE_DIRS'].split(':'):
            params.includes.append(include_dir)

        #import dependency libraries (should be in path)
        ctypes.CDLL("libgras.so", ctypes.RTLD_GLOBAL)
        ctypes.CDLL("libpmc.so", ctypes.RTLD_GLOBAL)

        op = grextras.ClangBlock(params)

        vec0 = numpy.array(numpy.random.randint(-150, +150, 10000), numpy.float32)
        vec1 = numpy.array(numpy.random.randint(-150, +150, 10000), numpy.float32)

        src0 = grextras.VectorSource(numpy.float32, vec0)
        src1 = grextras.VectorSource(numpy.float32, vec1)
        dst = grextras.VectorSink(numpy.float32)

        self.tb.connect(src0, (op, 0))
        self.tb.connect(src1, (op, 1))
        self.tb.connect(op, dst)
        self.tb.run()

        expected_result = list(vec0 + vec1)
        actual_result = list(dst.data())

        self.assertEqual(expected_result, actual_result)

    def test_add_const_float32(self):

        #setup clang block parameters
        params = grextras.ClangBlockParams()
        params.name = "make_add_const_float32"
        params.source = ADD_CONST_F32_SOURCE
        params.flags.append('-O3')

        #setup includes (set by test env var)
        for include_dir in os.environ['CLANG_BLOCK_INCLUDE_DIRS'].split(':'):
            params.includes.append(include_dir)

        #import dependency libraries (should be in path)
        ctypes.CDLL("libgras.so", ctypes.RTLD_GLOBAL)
        ctypes.CDLL("libpmc.so", ctypes.RTLD_GLOBAL)

        op = grextras.ClangBlock(params)
        offset = 42.
        op.set_value(offset) #set offset for test
        self.assertAlmostEqual(op.get_value(), offset)

        vec = numpy.array(numpy.random.randint(-150, +150, 10000), numpy.float32)

        src = grextras.VectorSource(numpy.float32, vec)
        dst = grextras.VectorSink(numpy.float32)

        self.tb.connect(src, op, dst)
        self.tb.run()

        expected_result = list(vec + offset)
        actual_result = list(dst.data())

        self.assertEqual(expected_result, actual_result)

if __name__ == '__main__':
    unittest.main()

