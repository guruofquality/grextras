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

#import pmt, first from local super tree if possible
try: import pmt
except ImportError: from gruel import pmt

from gnuradio import gr, gr_unittest
import block_gateway #needed to inject into gr

class demo_msg_src(gr.sync_block):
    def __init__(self, name, msgs):
        gr.sync_block.__init__(
            self,
            name = "demo msg src",
            in_sig = None,
            out_sig = None,
        )
        self._name = name
        self._msgs = msgs

    def work(self, input_items, output_items):
        # post all the msgs and be done...
        for msg in self._msgs:
            self.post_msg(
                self._name,
                pmt.pmt_string_to_symbol("example_key"),
                pmt.pmt_string_to_symbol(msg),
            )
        return -1

class demo_msg_sink(gr.sync_block):
    def __init__(self, name, num):
        gr.sync_block.__init__(
            self,
            name = "demo msg sink",
            in_sig = None,
            out_sig = None,
        )
        self._name = name
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
        src = demo_msg_src("foo", msgs)
        sink = demo_msg_sink("foo", len(msgs))
        tb.msg_connect(src, "foo", sink)
        tb.run()
        self.assertItemsEqual(sink.msgs(), msgs)

    def test_hier(self):
        msgs = ("this", "test", "should", "pass")
        tb = gr.top_block()

        #put the source in a hier block
        hb_src = gr.hier_block2("hb src", gr.io_signature(0, 0, 0), gr.io_signature(0, 0, 0))
        src = demo_msg_src("bar", msgs)
        hb_src.msg_connect(src, "bar", hb_src)

        #put the sink in a hier block
        hb_sink = gr.hier_block2("hb sink", gr.io_signature(0, 0, 0), gr.io_signature(0, 0, 0))
        sink = demo_msg_sink("bar", len(msgs))
        hb_sink.msg_connect(hb_sink, "bar", sink)

        #connect the hiers in the top level
        tb.msg_connect(hb_src, "bar", hb_sink)
        tb.run()
        self.assertItemsEqual(sink.msgs(), msgs)

    def test_disconnect(self):
        msgs = ("this", "test", "should", "pass")
        tb = gr.top_block()

        #put the source in a hier block
        hb_src = gr.hier_block2("hb src", gr.io_signature(0, 0, 0), gr.io_signature(0, 0, 0))
        src = demo_msg_src("bar", msgs)

        #put the sink in a hier block
        hb_sink = gr.hier_block2("hb sink", gr.io_signature(0, 0, 0), gr.io_signature(0, 0, 0))
        sink = demo_msg_sink("bar", len(msgs))

        #connect
        hb_src.msg_connect(src, "bar", hb_src)
        hb_sink.msg_connect(hb_sink, "bar", sink)
        tb.msg_connect(hb_src, "bar", hb_sink)

        #disconnect
        hb_src.msg_disconnect(src, "bar", hb_src)
        hb_sink.msg_disconnect(hb_sink, "bar", sink)
        tb.msg_disconnect(hb_src, "bar", hb_sink)

        #reconnect
        hb_src.msg_connect(src, "bar", hb_src)
        hb_sink.msg_connect(hb_sink, "bar", sink)
        tb.msg_connect(hb_src, "bar", hb_sink)

        tb.run()
        self.assertItemsEqual(sink.msgs(), msgs)

if __name__ == '__main__':
    gr_unittest.run(test_msg_passing, "test_msg_passing.xml")

