#
# Copyright 2004-2012 Free Software Foundation, Inc.
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
import grextras
import numpy

class test_signal_source(unittest.TestCase):

    def setUp(self):
        self.tb = gras.TopBlock()

    def tearDown(self):
        self.tb = None

    def test_signal_source_f32(self):
        expected_result = (1.5, 1.5, 1.5, 1.5, 1.5, 1.5, 1.5, 1.5, 1.5, 1.5)
        src1 = grextras.SignalSource.f32()
        src1.set_frequency(1e6, 0)
        src1.set_waveform("CONST")
        src1.set_amplitude(1.5)
        op = grextras.Head(numpy.float32, 10)
        dst1 = grextras.VectorSink(numpy.float32)
        self.tb.connect(src1, op, dst1)
        self.tb.run()
        dst_data = dst1.data()
        self.assertEqual(expected_result, dst_data)

if __name__ == '__main__':
    unittest.main()
