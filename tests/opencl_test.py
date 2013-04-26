# Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

import unittest
import gras
import grextras
import numpy

vector_add_gpu_SOURCE = """
__kernel void vector_add_gpu(
    __global const float* in0,
    __global const float* in1,
    __global float* out,
    __global uint *num_in_items,
    __global uint *num_out_items
)
{
    const uint i = get_global_id(0);

    if (i < num_out_items[0])
    {
        out[i] = in0[i] + in1[i];
    }

    //set minimum
    if (i == 0)
    {
        __private uint n = num_in_items[0];
        n = min(n, num_in_items[1]);
        n = min(n, num_out_items[0]);
        num_in_items[0] = n;
        num_in_items[1] = n;
        num_out_items[0] = n;
    }
}
"""

class test_opencl_block(unittest.TestCase):

    def setUp(self):
        self.tb = gras.TopBlock()

    def tearDown(self):
        self.tb = None

    def test_making_block(self):
        o = grextras.OpenClBlock("GPU")
        o.set_program("vector_add_gpu", vector_add_gpu_SOURCE)

if __name__ == '__main__':
    unittest.main()

