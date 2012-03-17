#!/usr/bin/env python
#
# Copyright 2004,2007,2010 Free Software Foundation, Inc.
# 
# This file is part of GNU Radio
# 
# GNU Radio is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3, or (at your option)
# any later version.
# 
# GNU Radio is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with GNU Radio; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street,
# Boston, MA 02110-1301, USA.
# 

from gnuradio import gr, gr_unittest
import math
import extras_swig as extras

class test_signal_source (gr_unittest.TestCase):

    def setUp (self):
        self.tb = gr.top_block ()

    def tearDown (self):
        self.tb = None

    def test_signal_source_f32 (self):
        tb = self.tb
        expected_result = (1.5, 1.5, 1.5, 1.5, 1.5, 1.5, 1.5, 1.5, 1.5, 1.5)
        src1 = extras.signal_source_f32()
        src1.set_frequency(1e6, 0)
        src1.set_waveform("CONST")
        src1.set_amplitude(1.5)
        op = gr.head (gr.sizeof_float, 10)
        dst1 = gr.vector_sink_f ()
        tb.connect (src1, op)
        tb.connect (op, dst1)
        tb.run ()
        dst_data = dst1.data ()
        self.assertEqual (expected_result, dst_data)

if __name__ == '__main__':
    gr_unittest.run(test_signal_source, "test_signal_source.xml")
