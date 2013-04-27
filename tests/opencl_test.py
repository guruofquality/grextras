# Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

import unittest
import gras
import grextras
import numpy

vector_add_gpu_SOURCE = """
__kernel void vector_add_gpu(
    __global const float* in0,
    __global const float* in1,
    __global float* out
)
{
    const uint i = get_global_id(0);
    out[i] = in0[i] + in1[i];
}
"""

class test_opencl_block(unittest.TestCase):

    def setUp(self):
        self.tb = gras.TopBlock()

    def tearDown(self):
        self.tb = None

    def test_making_block(self):
        op = grextras.OpenClBlock("GPU")
        op.set_program("vector_add_gpu", vector_add_gpu_SOURCE)
        op.input_config(0).item_size = 4
        op.output_config(0).item_size = 4

        vec0 = numpy.array(numpy.random.randint(-150, +150, 1e6), numpy.float32)
        vec1 = numpy.array(numpy.random.randint(-150, +150, 1e6), numpy.float32)

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

if __name__ == '__main__':
    unittest.main()

