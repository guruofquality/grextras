#
# Copyright 2007-2012 Free Software Foundation, Inc.
# Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.
#
# This file is part of GrExtras
#
# GrExtras is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3, or (at your option)
# any later version.
#
# GrExtras is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with GrExtras; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street,
# Boston, MA 02110-1301, USA.
#

import unittest
import gras
from gras import TestUtils
import numpy

class test_noise_source(unittest.TestCase):

    def setUp(self):
        self.tb = gras.TopBlock()

    def tearDown(self):
        self.tb = None

    def test_float32(self):
        op = gras.make('/extras/noise_source_f32', 0)
        op.set_waveform("GAUSSIAN")
        op.set_amplitude(10)

        head = TestUtils.Head(numpy.float32, 12)
        dst = TestUtils.VectorSink(numpy.float32)

        self.tb.connect(op, head, dst)
        self.tb.run()

        # expected results for Gaussian with seed 0, ampl 10
        expected_result =(-6.8885869979858398, 26.149959564208984,
                            20.575775146484375, -7.9340143203735352,
                            5.3359274864196777, -12.552099227905273,
                            6.333674430847168, -23.830753326416016,
                            -16.603046417236328, 2.9676761627197266,
                            1.2176077365875244, 15.100193977355957)

        dst_data = dst.data()
        self.assertEqual(len(expected_result), len(dst_data))
        for i in range(len(dst_data)):
            self.assertAlmostEqual(expected_result[i], dst_data[i], places=3)


if __name__ == '__main__':
    unittest.main()

