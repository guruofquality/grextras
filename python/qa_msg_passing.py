#!/usr/bin/env python
#
# Copyright 2011-2012 Free Software Foundation, Inc.
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

#hacky way so we can import in-tree
try: import pmt
except ImportError: from gruel import pmt

from gnuradio import gr, gr_unittest
import block_gateway #needed to inject into gr

class demo_msg_src(gr.block):
    def __init__(self, msgs):
        gr.block.__init__(
            self,
            name = "demo msg src",
            in_sig = None,
            out_sig = None,
            num_msg_outputs = 1,
        )
        self._msgs = msgs

    def work(self, input_items, output_items):
        # post all the msgs and be done...
        for msg in self._msgs:
            self.post_msg(
                0,
                pmt.pmt_string_to_symbol("example_key"),
                pmt.pmt_string_to_symbol(msg),
            )
        #FIXME, wait for sink thread to get msgs
        #if we return -1 too quickly, everything kind of shuts down b4 the messages get there...
        import time
        time.sleep(.1)
        return -1

class demo_msg_sink(gr.block):
    def __init__(self, num):
        gr.block.__init__(
            self,
            name = "demo msg sink",
            in_sig = None,
            out_sig = None,
            has_msg_input = True
        )
        self._msgs = []
        self._num = num

    def msgs(self): return self._msgs

    def work(self, input_items, output_items):
        while True:
            msg = self.pop_msg_queue()
            self._msgs.append(pmt.pmt_symbol_to_string(msg.value))
            if len(self._msgs) == self._num: return -1

class test_msg_passing(gr_unittest.TestCase):

    def test_top(self):
        msgs = ("this", "test", "should", "pass")
        tb = gr.top_block()
        src = demo_msg_src(msgs)
        sink = demo_msg_sink(len(msgs))
        tb.connect(src, sink)
        tb.run()
        self.assertItemsEqual(sink.msgs(), msgs)

    def test_hier(self):
        msgs = ("this", "test", "should", "pass")
        tb = gr.top_block()

        #put the source in a hier block
        hb_src = gr.hier_block2("hb src", gr.io_signature(0, 0, 0), gr.io_signature(1, 1, 1))
        src = demo_msg_src(msgs)
        hb_src.connect(src, hb_src)

        #put the sink in a hier block
        hb_sink = gr.hier_block2("hb sink", gr.io_signature(1, 1, 1), gr.io_signature(0, 0, 0))
        sink = demo_msg_sink(len(msgs))
        hb_sink.connect(hb_sink, sink)

        #connect the hiers in the top level
        tb.connect(hb_src, hb_sink)
        tb.run()
        self.assertItemsEqual(sink.msgs(), msgs)

    def test_disconnect(self):
        msgs = ("this", "test", "should", "pass")
        tb = gr.top_block()

        #put the source in a hier block
        hb_src = gr.hier_block2("hb src", gr.io_signature(0, 0, 0), gr.io_signature(1, 1, 1))
        src = demo_msg_src(msgs)

        #put the sink in a hier block
        hb_sink = gr.hier_block2("hb sink", gr.io_signature(1, 1, 1), gr.io_signature(0, 0, 0))
        sink = demo_msg_sink(len(msgs))

        #connect
        hb_src.connect(src, hb_src)
        hb_sink.connect(hb_sink, sink)
        tb.connect(hb_src, hb_sink)

        #disconnect
        hb_src.disconnect(src, hb_src)
        hb_sink.disconnect(hb_sink, sink)
        tb.disconnect(hb_src, hb_sink)

        #reconnect
        hb_src.connect(src, hb_src)
        hb_sink.connect(hb_sink, sink)
        tb.connect(hb_src, hb_sink)

        tb.run()
        self.assertItemsEqual(sink.msgs(), msgs)

if __name__ == '__main__':
    gr_unittest.run(test_msg_passing, "test_msg_passing.xml")

