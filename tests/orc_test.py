# Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

import unittest
import gras
import grextras
import numpy

SOURCE = """
.function volk_32f_x2_add_32f_a_orc_impl
.dest 4 dst
.source 4 src1
.source 4 src2
addf dst, src1, src2

.function volk_32f_sqrt_32f_a_orc_impl
.source 4 src
.dest 4 dst
sqrtf dst, src
"""

class test_orc_block(unittest.TestCase):

    def setUp(self):
        self.tb = gras.TopBlock()

    def tearDown(self):
        self.tb = None

    def test_add_float32(self):
        op = grextras.OrcBlock()
        op.set_program("volk_32f_x2_add_32f_a_orc_impl", SOURCE)
        op.input_config(0).item_size = 4
        op.output_config(0).item_size = 4

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

    def test_sqrt_float32(self):
        op = grextras.OrcBlock()
        op.set_program("volk_32f_sqrt_32f_a_orc_impl", SOURCE)
        op.input_config(0).item_size = 4
        op.output_config(0).item_size = 4

        vec = numpy.array(numpy.random.randint(10, +150, 10000), numpy.float32)

        src = grextras.VectorSource(numpy.float32, vec)
        dst = grextras.VectorSink(numpy.float32)

        self.tb.connect(src, op, dst)
        self.tb.run()

        expected_result = list(numpy.sqrt(vec))
        actual_result = list(dst.data())

        self.assertEqual(expected_result, actual_result)

if __name__ == '__main__':
    unittest.main()

