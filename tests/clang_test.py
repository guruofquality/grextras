# Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

import unittest
import gras
import grextras
import numpy

SOURCE = """
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

    void notify_topology(const size_t num_ins, const size_t num_outs)
    {
        std::cout << "num_ins " << num_ins << std::endl;
        std::cout << "num_outs " << num_outs << std::endl;
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

class test_clang_block(unittest.TestCase):

    def setUp(self):
        self.tb = gras.TopBlock()

    def tearDown(self):
        self.tb = None

    def test_add_float32(self):
        import ctypes
        ctypes.CDLL("/home/jblum/build/gras/lib/libgras.so", ctypes.RTLD_GLOBAL)
        ctypes.CDLL("/home/jblum/build/gras/PMC/lib/libpmc.so", ctypes.RTLD_GLOBAL)

        params = grextras.ClangBlockParams()
        params.name = "make_add_float32"
        params.code = SOURCE
        #params.flags.append('-stdlib=libstdc++')
        params.include_dirs.append('/home/jblum/src/gras/include/')
        params.include_dirs.append('/home/jblum/src/gras/PMC/include/')
        op = grextras.ClangBlock(params)

        vec0 = numpy.array(numpy.random.randint(-150, +150, 100), numpy.float32)
        vec1 = numpy.array(numpy.random.randint(-150, +150, 100), numpy.float32)

        src0 = grextras.VectorSource(numpy.float32, vec0)
        src1 = grextras.VectorSource(numpy.float32, vec1)
        dst = grextras.VectorSink(numpy.float32)

        self.tb.connect(src0, (op, 0))
        self.tb.connect(src1, (op, 1))
        self.tb.connect(op, dst)
        self.tb.run()

        expected_result = list(vec0 + vec1)
        actual_result = list(dst.data())

        print actual_result
        self.assertEqual(expected_result, actual_result)

if __name__ == '__main__':
    unittest.main()

