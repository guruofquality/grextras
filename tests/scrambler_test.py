# Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

import unittest
import gras
import grextras
import numpy
import os
import ctypes
import random

class test_scramblers(unittest.TestCase):

    def setUp(self):
        self.tb = gras.TopBlock()

    def tearDown(self):
        self.tb = None

    def test_simple_loopback(self):
        for i in range(16):
            s = grextras.Scrambler()
            d = grextras.Descrambler()

            poly = ctypes.c_int64(1 | (1 << 14) | (1 << 15))
            seed = ctypes.c_int64(random.randint(-1024, 1024))
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

    def test_sync_word(self):
        if 1:
            s = grextras.Scrambler()
            d = grextras.Descrambler()
            poly = ctypes.c_int64(1 | (1 << 14) | (1 << 15))
            seed = ctypes.c_int64(random.randint(-1024, 1024))
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
            self.tb.disconnect_all()
            #lengths wont equal, sync word does history thing
            n = min(len(src_data), len(dst.data()))
            self.assertEqual(src_data[:n], dst.data()[:n])

if __name__ == '__main__':
    unittest.main()

