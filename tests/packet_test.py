# Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

import unittest
import gras
import grextras
import numpy
import time

class test_packet(unittest.TestCase):

    def setUp(self):
        self.tb = gras.TopBlock()

    def tearDown(self):
        self.tb = None

    def test_simple_loopback(self):
        framer = grextras.PacketFramer(
            samples_per_symbol = 2,
            bits_per_symbol = 1,
        )
        deframer = grextras.PacketDeframer()

        src_data = tuple(numpy.random.randint(-1024, 1024, 10))
        src = grextras.VectorSource(numpy.int32, src_data, autodone=False)
        dst = grextras.VectorSink(numpy.int32)

        from gnuradio import gr
        unpack = gr.packed_to_unpacked_bb(1, gr.GR_MSB_FIRST)

        s2d = grextras.Stream2Datagram(numpy.dtype(numpy.int32).itemsize)
        d2s = grextras.Datagram2Stream(numpy.dtype(numpy.int32).itemsize)

        self.tb.connect(src, s2d, framer, unpack, deframer, d2s, dst)

        self.tb.start()
        time.sleep(0.1)
        print 'stop!!!!!!!!!!!!!!'
        self.tb.stop()
        self.tb.wait()
        self.tb.disconnect_all()

        self.assertEqual(src_data, dst.data())

if __name__ == '__main__':
    try: #packet framer has a gnuradio dependency, check import before failing the test
        import gnuradio
        unittest.main()
    except ImportError:
        exit()
