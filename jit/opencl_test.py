# Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

import unittest
import gras
import TestUtils
import numpy

vector_add_gpu_SOURCE = """
__kernel void add_2x_float32(
    __global const float* in0,
    __global const float* in1,
    __global float* out
)
{
    const uint i = get_global_id(0);
    out[i] = in0[i] + in1[i];
}

__kernel void add_2x_complex64(
    __global const float2* in0,
    __global const float2* in1,
    __global float2* out
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

    def test_add_float32(self):
        op = gras.make('/extras/opencl_block', "")
        op.set_program("add_2x_float32", vector_add_gpu_SOURCE, "")
        op.set_input_size(0, 4)
        op.set_output_size(0, 4)

        vec0 = numpy.array(numpy.random.randint(-150, +150, 1e6), numpy.float32)
        vec1 = numpy.array(numpy.random.randint(-150, +150, 1e6), numpy.float32)

        src0 = TestUtils.VectorSource(numpy.float32, vec0)
        src1 = TestUtils.VectorSource(numpy.float32, vec1)
        dst = TestUtils.VectorSink(numpy.float32)

        self.tb.connect(src0, (op, 0))
        self.tb.connect(src1, (op, 1))
        self.tb.connect(op, dst)
        self.tb.run()

        expected_result = list(vec0 + vec1)
        actual_result = list(dst.data())

        self.assertEqual(expected_result, actual_result)

    def test_add_complex64(self):
        op = gras.make('/extras/opencl_block', "")
        op.set_program("add_2x_complex64", vector_add_gpu_SOURCE, "")
        op.set_input_size(0, 8)
        op.set_output_size(0, 8)

        vec0 = numpy.array(numpy.random.randint(-150, +150, 1e6), numpy.complex64)
        vec1 = numpy.array(numpy.random.randint(-150, +150, 1e6), numpy.complex64)

        src0 = TestUtils.VectorSource(numpy.complex64, vec0)
        src1 = TestUtils.VectorSource(numpy.complex64, vec1)
        dst = TestUtils.VectorSink(numpy.complex64)

        self.tb.connect(src0, (op, 0))
        self.tb.connect(src1, (op, 1))
        self.tb.connect(op, dst)
        self.tb.run()

        expected_result = list(vec0 + vec1)
        actual_result = list(dst.data())

        self.assertEqual(expected_result, actual_result)

if __name__ == '__main__':
    unittest.main()

