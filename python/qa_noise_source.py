#!/usr/bin/env python
#
# Copyright 2007,2010 Free Software Foundation, Inc.
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
import extras_swig as extras

class test_noise_source(gr_unittest.TestCase):

    def setUp (self):
        self.tb = gr.top_block ()

    def tearDown (self):
        self.tb = None

    def test_001(self):
        op = extras.noise_source_f32(0)
        op.set_waveform("GAUSSIAN")
        op.set_amplitude(10)

        head = gr.head(gr.sizeof_float, 12)
        dst = gr.vector_sink_f()

        tb = gr.top_block()
        tb.connect(op, head, dst)
        tb.run()

        # expected results for Gaussian with seed 0, ampl 10
        expected_result = (-6.8885869979858398, 26.149959564208984,
                            20.575775146484375, -7.9340143203735352,
                            5.3359274864196777, -12.552099227905273,
                            6.333674430847168, -23.830753326416016,
                            -16.603046417236328, 2.9676761627197266,
                            1.2176077365875244, 15.100193977355957)

        dst_data = dst.data ()
        self.assertEqual (expected_result, dst_data)
        

if __name__ == '__main__':
    gr_unittest.run(test_noise_source, "test_noise_source.xml")
        
