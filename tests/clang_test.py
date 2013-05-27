# Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

import unittest
import gras
import grextras
import numpy

SOURCE = """
#include <stdio.h>
#include <iostream>

#include <gras/block.hpp>

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

gras::Block *make(void)
{
    printf("makes hello\\n");
    return new MyAddFloat32();
}
"""

class test_clang_block(unittest.TestCase):

    def setUp(self):
        self.tb = gras.TopBlock()

    def tearDown(self):
        self.tb = None

    def test_foo(self):

        import ctypes
        ctypes.CDLL("/home/jblum/build/gras/lib/libgras.so", ctypes.RTLD_GLOBAL)
        ctypes.CDLL("/home/jblum/build/gras/PMC/lib/libpmc.so", ctypes.RTLD_GLOBAL)

        params = grextras.ClangBlockParams()
        params.name = "make"
        #params.flags.append('-stdlib=libstdc++')
        params.code = SOURCE
        params.include_dirs.append('/home/jblum/src/gras/include/')
        params.include_dirs.append('/home/jblum/src/gras/PMC/include/')
        cb = grextras.ClangBlock(params)

if __name__ == '__main__':
    unittest.main()

