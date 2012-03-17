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

class test_delay(gr_unittest.TestCase):

    def setUp (self):
        self.tb = gr.top_block ()

    def tearDown (self):
        self.tb = None

    def test_001(self):
        delay = 4
        src_data = [1, 2, 3, 4, 5, 6, 7, 8]
        expected_result = (0, 0, 0, 0, 1, 2, 3, 4, 5, 6, 7, 8)

        src = gr.vector_source_f(src_data)
        op = extras.delay(gr.sizeof_float)
        op.set_delay(delay)
        dst = gr.vector_sink_f()

        tb = gr.top_block()
        tb.connect(src, op, dst)
        tb.run()

        dst_data = dst.data ()
        self.assertEqual (expected_result, dst_data)
        

if __name__ == '__main__':
    gr_unittest.run(test_delay, "test_delay.xml")
        
