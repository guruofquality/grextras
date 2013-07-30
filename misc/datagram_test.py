# Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

import unittest
import gras
import TestUtils
import numpy
import time

class test_datagram(unittest.TestCase):

    def setUp(self):
        self.tb = gras.TopBlock()

    def tearDown(self):
        self.tb = None

    def test_simple_loopback(self):
        src_data = (8, 3, 32, 18, -53)
        src = TestUtils.VectorSource(numpy.int32, src_data)
        dst = TestUtils.VectorSink(numpy.int32)

        s2d = gras.make('/extras/stream_to_datagram', numpy.dtype(numpy.float32).itemsize, 0)
        d2s = gras.make('/extras/datagram_to_stream', numpy.dtype(numpy.float32).itemsize)

        self.tb.connect(src, s2d, d2s, dst)
        self.tb.run()

        self.assertEqual(src_data, dst.data())

if __name__ == '__main__':
    unittest.main()
