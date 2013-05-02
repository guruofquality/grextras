# Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

import unittest
import gras
import grextras
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

__kernel void invert_float32(
    __global const float* in,
    __global float* out
)
{
    const uint i = get_global_id(0);
    out[i] = -in[i];
}
"""

class test_opencl_block(unittest.TestCase):

    def setUp(self):
        self.tb = gras.TopBlock()

    def tearDown(self):
        self.tb = None
    """
    def test_add_float32(self):
        op = grextras.OpenClBlock("GPU")
        op.set_program("add_2x_float32", vector_add_gpu_SOURCE)
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

    def test_add_complex64(self):
        op = grextras.OpenClBlock("GPU")
        op.set_program("add_2x_complex64", vector_add_gpu_SOURCE)
        op.input_config(0).item_size = 8
        op.output_config(0).item_size = 8

        vec0 = numpy.array(numpy.random.randint(-150, +150, 1e6), numpy.complex64)
        vec1 = numpy.array(numpy.random.randint(-150, +150, 1e6), numpy.complex64)

        src0 = grextras.VectorSource(numpy.complex64, vec0)
        src1 = grextras.VectorSource(numpy.complex64, vec1)
        dst = grextras.VectorSink(numpy.complex64)

        self.tb.connect(src0, (op, 0))
        self.tb.connect(src1, (op, 1))
        self.tb.connect(op, dst)
        self.tb.run()

        expected_result = list(vec0 + vec1)
        actual_result = list(dst.data())

        self.assertEqual(expected_result, actual_result)
    """

    def test_add_float32_and_invert(self):
        """
        This tests the ability to chain open cl blocks without going to host memory.
        """
        add = grextras.OpenClBlock("GPU")
        add.set_program("add_2x_float32", vector_add_gpu_SOURCE)
        add.input_config(0).item_size = 4
        add.output_config(0).item_size = 4
        add.set_access_mode("OUTPUT", ["DO"]) #device only

        inv = grextras.OpenClBlock("GPU")
        inv.set_program("invert_float32", vector_add_gpu_SOURCE)
        inv.input_config(0).item_size = 4
        inv.output_config(0).item_size = 4
        inv.set_access_mode("INPUT", ["DO"]) #device only

        vec0 = numpy.array(numpy.random.randint(-150, +150, 1e6), numpy.float32)
        vec1 = numpy.array(numpy.random.randint(-150, +150, 1e6), numpy.float32)

        src0 = grextras.VectorSource(numpy.float32, vec0)
        src1 = grextras.VectorSource(numpy.float32, vec1)
        dst = grextras.VectorSink(numpy.float32)

        self.tb.connect(src0, (add, 0))
        self.tb.connect(src1, (add, 1))
        self.tb.connect(add, inv, dst)
        self.tb.run()

        expected_result = list(-1.0*(vec0 + vec1))
        actual_result = list(dst.data())

        self.assertEqual(expected_result, actual_result)

if __name__ == '__main__':
    unittest.main()

