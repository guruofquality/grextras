# Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

import unittest
import gras
import numpy
import TestUtils

class test_delay(unittest.TestCase):

    def test_delay_simple(self):
        for delay in (0, 100, 1000, 10000):
            src_data = [1, 2, 3, 4, 5, 6, 7, 8]
            expected_result = tuple([0]*delay + src_data)

            src = TestUtils.VectorSource(numpy.float32, src_data)
            op = gras.make('/extras/delay', numpy.dtype(numpy.float32).itemsize)
            op.set_delay(delay)
            dst = TestUtils.VectorSink(numpy.float32)

            tb = gras.TopBlock()
            tb.connect(src, op, dst)
            tb.run()

            dst_data = dst.data()
            self.assertEqual(expected_result, dst_data)

    #TODO test tag propagation
    #TODO test live change

if __name__ == '__main__':
    unittest.main()
