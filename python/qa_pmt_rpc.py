#!/usr/bin/env python
#
# Copyright 2012 Free Software Foundation, Inc.
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
import extras_pmt #injects
import pmt_to_python #injects
from pmt_rpc import pmt_rpc

#hacky way so we can import in-tree
try: import pmt
except ImportError: from gruel import pmt

my_mini_prog = """#!
arg='hey this works'
"""

class control_block(gr.block):
    def __init__(self):
        gr.block.__init__(
            self,
            name = "demo control block",
            in_sig = None,
            out_sig = None,
            num_msg_outputs = 1,
            has_msg_input = True,
        )

    def test_print(self, arg):
        print arg

    def work(self, input_items, output_items):
        self.post_msg(0, pmt.from_python("multx2"), pmt.from_python(((42,), None)))

        msg = self.pop_msg_queue()
        assert(pmt.to_python(msg.key) == "multx2")
        request, result, error = pmt.to_python(msg.value)

        print(error)
        assert(not error)
        print(result)
        assert(result == 42*2)

        self.post_msg(0, pmt.from_python("_control_block.test_print"), pmt.from_python((('hello',), None)))
        self.post_msg(0, pmt.from_python("_control_block.test_print"), pmt.from_python(((my_mini_prog,), None)))

        return -1

class my_top_block(gr.top_block):
    def __init__(self):
        gr.top_block.__init__(self)
        self._pmt_rpc = pmt_rpc(self, result_msg=True)
        self._control_block = control_block()
        self.connect(self._control_block, self._pmt_rpc) #connect rpc request
        self.connect(self._pmt_rpc, self._control_block) #connect rpc result

    def multx2(self, num):
        print 'mult!!!'
        return num*2

class test_pmt_rpc(gr_unittest.TestCase):

    def test_it(self):
        tb = my_top_block()
        tb.start()
        import time
        time.sleep(1)
        tb.stop()
        tb.wait()

if __name__ == '__main__':
    gr_unittest.run(test_pmt_rpc, "test_pmt_rpc.xml")
