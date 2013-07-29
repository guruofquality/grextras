# Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

import unittest
import gras
import TestUtils
import numpy
import time

import gmsk
class TagSink(gras.Block):
    def __init__(self):
        gras.Block.__init__(self,
            name = "TagSink",
            in_sig = [numpy.complex64],
        )
        self._results = list()

    def work(self, ins, outs):
        if len(ins[0]): self.consume(0, len(ins[0]))

    def propagate_tags(self, index, tag_iter):
        for tag in tag_iter:
            st = tag.object()
            self._results.append((tag.offset, st.key(), st.val()))

    def data(self):
        return self._results

class test_packet(unittest.TestCase):

    def setUp(self):
        self.tb = gras.TopBlock()

    def tearDown(self):
        self.tb = None

    def test_simple_loopback(self):
        framer = gras.Factory.make('/extras/packet_framer',
            samples_per_symbol = 2,
            bits_per_symbol = 1,
        )
        deframer = gras.Factory.make('/extras/packet_deframer')

        src_data = tuple(numpy.random.randint(-1024, 1024, 11)) #40 bytes + padding
        src = TestUtils.VectorSource(numpy.int32, src_data)
        dst = TestUtils.VectorSink(numpy.int32)

        from gnuradio import gr
        unpack = gr.packed_to_unpacked_bb(1, gr.GR_MSB_FIRST)

        s2d = gras.Factory.make('/extras/stream_to_datagram', numpy.dtype(numpy.int32).itemsize, 40) #mtu 40 bytes
        d2s = gras.Factory.make('/extras/datagram_to_stream', numpy.dtype(numpy.int32).itemsize)

        self.tb.connect(src, s2d, framer, unpack, deframer, d2s, dst)

        self.tb.start()
        time.sleep(1.0)
        self.tb.stop()
        self.tb.wait()
        self.tb.disconnect_all()

        print "src_data", src_data
        print "dst_data", dst.data()

        n = min(map(len, (src_data, dst.data())))
        print n
        self.assertTrue(n > 0)

        self.assertEqual(src_data[:n], dst.data()[:n])

    def test_with_phy(self):

        sps = 4

        mod = gmsk.gmsk_mod(
            samples_per_symbol=sps,
            bt=0.35,
            verbose=False,
            log=False,
        )

        demod = gmsk.gmsk_demod(
            samples_per_symbol=sps,
            gain_mu=0.175,
            mu=0.5,
            omega_relative_limit=0.005,
            freq_error=0.0,
            verbose=False,
            log=False,
        )

        framer = gras.Factory.make('/extras/packet_framer',
            samples_per_symbol = sps,
            bits_per_symbol = 1,
        )

        burst_tagger = gras.Factory.make('/extras/burst_tagger', sps)

        deframer = gras.Factory.make('/extras/packet_deframer')

        src_data = tuple(numpy.random.randint(-1024, 1024, 11)) #40 bytes + padding
        src = TestUtils.VectorSource(numpy.int32, src_data)
        dst = TestUtils.VectorSink(numpy.int32)

        s2d = gras.Factory.make('/extras/stream_to_datagram', numpy.dtype(numpy.int32).itemsize, 40) #mtu 40 bytes
        d2s = gras.Factory.make('/extras/datagram_to_stream', numpy.dtype(numpy.int32).itemsize)

        delay = gras.Factory.make('/extras/delay', numpy.dtype(numpy.complex64).itemsize)
        delay.set_delay(73) #sample delay

        #inject noise into the system
        noise = gras.Factory.make('/extras/noise_source_fc32', -1)
        noise.set_amplitude(0.05)
        add = gras.Factory.make('/extras/add_fc32_fc32')
        self.tb.connect(noise, (add, 1))

        self.tb.connect(src, s2d, framer, mod, burst_tagger, delay, add, demod, deframer, d2s, dst)

        #collect tags
        tag_sink = TagSink()
        self.tb.connect(add, tag_sink)

        self.tb.start()
        time.sleep(1.0)
        self.tb.stop()
        self.tb.wait()
        self.tb.disconnect_all()

        print "src_data", src_data
        print "dst_data", dst.data()
        print "tag_sink", tag_sink.data()

        n = min(map(len, (src_data, dst.data())))
        print n
        self.assertTrue(n > 0)

        self.assertEqual(src_data[:n], dst.data()[:n])

if __name__ == '__main__':
    try: #packet framer has a gnuradio dependency, check import before failing the test
        import gnuradio
        unittest.main()
    except ImportError:
        exit()
