# Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

import unittest
import gras
import grextras
import numpy
import time

class test_datagram(unittest.TestCase):

    def setUp(self):
        self.tb = gras.TopBlock()

    def tearDown(self):
        self.tb = None

    def test_simple_loopback(self):
        src_data = (8, 3, 32, 18, -53)
        src = grextras.VectorSource(numpy.int32, src_data, autodone=False)
        dst = grextras.VectorSink(numpy.int32)

        s2d = grextras.Stream2Datagram(numpy.dtype(numpy.float32).itemsize)
        d2s = grextras.Datagram2Stream(numpy.dtype(numpy.float32).itemsize)

        self.tb.connect(src, s2d, d2s, dst)
        self.tb.start()
        time.sleep(0.1)
        self.tb.stop()
        self.tb.wait()

        self.assertEqual(src_data, dst.data())

if __name__ == '__main__':
    unittest.main()
