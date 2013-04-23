# Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

import unittest
import gras
import grextras
import numpy

vector_add_gpu_SOURCE = """
__kernel void vector_add_gpu (__global const float* src_a,
                     __global const float* src_b,
                     __global float* res,
                    const int num)
{
   /* get_global_id(0) returns the ID of the thread in execution.
   As many threads are launched at the same time, executing the same kernel,
   each one will receive a different ID, and consequently perform a different computation.*/
   const int idx = get_global_id(0);

   /* Now each work-item asks itself: "is my ID inside the vector's range?"
   If the answer is YES, the work-item performs the corresponding computation*/
   if (idx < num)
      res[idx] = src_a[idx] + src_b[idx];
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

