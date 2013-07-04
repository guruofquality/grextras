# Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

import unittest
import gras
import grextras
import numpy
import os
import random
import ctypes
from PMC import *

class LengthTagSource(gras.Block):

    def __init__(self, src_data):
        gras.Block.__init__(self, "LengthTagSource", out_sig=[numpy.uint8])
        self._src_data = src_data
        self._do_len_tag = True

    def work(self, ins, outs):
        src0 = self._src_data[0]
        len0 = len(src0)

        if self._do_len_tag:
            self._do_len_tag = False
            st = gras.StreamTag(PMC_M("length"), PMC_M(ctypes.c_size_t(len0)))
            self.post_output_tag(0, gras.Tag(self.get_produced(0), PMC_M(st)))

        n = min(len0, len(outs[0]))
        outs[0][:n] = src0[:n]
        self.consume(n)
        self.produce(n)
        self._src_data[0] = self._src_data[0][n:]

        if not self._src_data[0]:
            self._src_data = self._src_data[1:]
            self._do_len_tag = True
        if not self._src_data:
            self.mark_done()

class test_scramblers(unittest.TestCase):

    def setUp(self):
        self.tb = gras.TopBlock()

    def tearDown(self):
        self.tb = None

    def test_simple_loopback(self):
        for i in range(16):
            s = grextras.Scrambler()
            d = grextras.Descrambler()

            poly = 1 | (1 << 14) | (1 << 15)
            seed = random.randint(-1024, 1024)
            mode = random.choice(("additive", "multiplicative"))
            print poly, seed, mode
            for obj in (s, d):
                obj.set("poly", poly)
                obj.set("seed", seed)
                obj.set("mode", mode)

            nbits = random.randint(128, 4096)
            src_data = tuple(numpy.random.randint(0, 2, nbits))
            src = grextras.VectorSource(numpy.uint8, src_data)
            dst = grextras.VectorSink(numpy.uint8)
            self.tb.connect(src, s, d, dst)

            midst = grextras.VectorSink(numpy.uint8)
            self.tb.connect(s, midst)

            self.tb.run()
            self.tb.disconnect_all()
            self.assertNotEqual(src_data, midst.data())
            self.assertEqual(src_data, dst.data())

    def test_sync_word_length_not_used(self):
        if 1:
            s = grextras.Scrambler()
            d = grextras.Descrambler()
            poly = 1 | (1 << 14) | (1 << 15)
            seed = random.randint(-1024, 1024)
            mode = random.choice(("additive", "multiplicative"))
            sync = "111100111100000111"
            print poly, seed, mode
            for obj in (s, d):
                obj.set("poly", poly)
                obj.set("seed", seed)
                obj.set("mode", mode)
                obj.set("sync", sync)

            nbits = random.randint(128, 4096)
            src_data = tuple(numpy.random.randint(0, 2, nbits))
            src = grextras.VectorSource(numpy.uint8, src_data)
            dst = grextras.VectorSink(numpy.uint8)
            self.tb.connect(src, s, d, dst)

            self.tb.run()
            #self.tb.disconnect_all()
            #lengths wont equal, sync word does history thing
            n = min(len(src_data), len(dst.data()))
            self.assertEqual(src_data[:n], dst.data()[:n])

    def test_sync_word(self):
        if 1:
            s = grextras.Scrambler()
            d = grextras.Descrambler()
            poly = 1 | (1 << 14) | (1 << 15)
            seed = random.randint(-1024, 1024)
            mode = random.choice(("additive", "multiplicative"))
            sync = "111100111100000111"
            print poly, seed, mode
            for obj in (s, d):
                obj.set("poly", poly)
                obj.set("seed", seed)
                obj.set("mode", mode)
                obj.set("sync", sync)

            src_bursts = list()
            for it in range(7):
                src_bursts.append(numpy.random.randint(0, 2, random.randint(128, 4096)))
            src_data = tuple(numpy.concatenate(src_bursts))

            src = LengthTagSource(src_bursts)
            dst = grextras.VectorSink(numpy.uint8)
            self.tb.connect(src, s, d, dst)

            self.tb.run()
            #self.tb.disconnect_all()
            #lengths wont equal, sync word does history thing
            n = min(len(src_data), len(dst.data()))
            self.assertEqual(src_data[:n], dst.data()[:n])

if __name__ == '__main__':
    unittest.main()

