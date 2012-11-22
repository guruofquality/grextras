# Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

import unittest
import gras
import grextras
import numpy
import time

class test_stream_selector(unittest.TestCase):

    def test_stream_selector_simple(self):
        ss = grextras.StreamSelector(numpy.dtype(numpy.float32).itemsize)
        ss.set_paths([1, 0]) #in[0] -> out[1], in[1] -> out[0]

        src0 = grextras.VectorSource(numpy.float32, [1, 2, 3, 4], autodone=False)
        src1 = grextras.VectorSource(numpy.float32, [5, 6, 7, 8], autodone=False)

        dst0 = grextras.VectorSink(numpy.float32)
        dst1 = grextras.VectorSink(numpy.float32)

        tb = gras.TopBlock()
        tb.connect(src0, (ss, 0), dst0)
        tb.connect(src1, (ss, 1), dst1)
        tb.start()
        time.sleep(0.1)
        tb.stop()
        tb.wait()

        self.assertEqual((1, 2, 3, 4), dst1.data())
        self.assertEqual((5, 6, 7, 8), dst0.data())

    #TODO test non forwarding inputs
    #TODO test tag propagation
    #TODO test live change

if __name__ == '__main__':
    unittest.main()
