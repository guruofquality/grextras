#!/usr/bin/env python
#
# Copyright 2011 Free Software Foundation, Inc.
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

import numpy
import pmt #from gruel import pmt
from gnuradio import gr, gr_unittest

class test_blob_stream(gr_unittest.TestCase):

    def test_000(self):
        tb = gr.top_block()
        src = gr.vector_source_f([1, 2, 3, 4, 5, 6, 7, 8], False)
        s2b = gr.stream_to_blob(4)
        b2s = gr.blob_to_stream(4)
        sink = gr.vector_sink_f()
        tb.connect(src, s2b)
        tb.msg_connect(s2b, "blob", b2s)
        tb.connect(b2s, sink)
        tb.run()
        self.assertItemsEqual(sink.data(), (1, 2, 3, 4, 5, 6, 7, 8))

    def test_random(self):
        for num_items in numpy.random.randint(5000, 10000, 10):
            rand_int_arr = numpy.array(numpy.random.randint(0, 2**15, num_items), numpy.int16)
            tb = gr.top_block()
            src = gr.vector_source_s(map(int, rand_int_arr), False)
            s2b = gr.stream_to_blob(2)
            b2s = gr.blob_to_stream(2)
            sink = gr.vector_sink_s()
            tb.connect(src, s2b)
            tb.msg_connect(s2b, "blob", b2s)
            tb.connect(b2s, sink)
            tb.run()
            self.assertItemsEqual(sink.data(), rand_int_arr)

    def test_slicer(self):
        for num_items in (4000, 4500, 5000, 5500, 6000):
            rand_int_arr = numpy.array(numpy.random.randint(0, 2**15, num_items), numpy.int16)
            tb = gr.top_block()
            src = gr.vector_source_s(map(int, rand_int_arr), False)
            s2b = gr.stream_to_blob(2, 100) #slice of size 100 bytes
            b2s = gr.blob_to_stream(2)
            sink = gr.vector_sink_s()
            tb.connect(src, s2b)
            tb.msg_connect(s2b, "blob", b2s)
            tb.connect(b2s, sink)
            tb.run()
            self.assertItemsEqual(sink.data(), rand_int_arr)

if __name__ == '__main__':
    gr_unittest.run(test_blob_stream, "test_blob_stream.xml")

