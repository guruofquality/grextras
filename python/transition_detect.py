#
# Copyright 1980-2012 Free Software Foundation, Inc.
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
from math import pi
from gnuradio import gr
from gruel import pmt
from gnuradio.digital import packet_utils
import gnuradio.digital as gr_digital
import block_gateway #needed to inject into gr

# /////////////////////////////////////////////////////////////////////////////
#                   transition_detect - output msg with value when byte stream changes
# /////////////////////////////////////////////////////////////////////////////

class transition_detect(gr.block):
    """
    Wrap an arbitrary digital modulator in our packet handling framework.

    Send packets by calling send_pkt
    """
    def __init__(
        self,
    ):
        """
        The input is a stream of bytes.
        Outputs a blob with value of current byte if it is different from last.
        """

		
        gr.block.__init__(
            self,
            name = "transition_detect",
            in_sig = [numpy.uint8],
            out_sig = None,
            has_msg_input = False,
            num_msg_outputs = 1,
            
        )
    
        self.mgr = pmt.pmt_mgr()
        for i in range(64):
			self.mgr.set(pmt.pmt_make_blob(10000))
        self.old_result = 0;
        
        self.key = pmt.pmt_string_to_symbol("example_key")
        self.value = pmt.pmt_string_to_symbol("example_value")
        self.trans_count = 0
 
    def work(self, input_items, output_items):
		in0 = input_items[0]
		for x in range(len(in0)):
			if self.old_result != in0[x]:
				self.value = [in0[x]]
				self.trans_count += 1
				blob = self.mgr.acquire(True) #block
				pmt.pmt_blob_resize(blob, len(self.value))
				pmt.pmt_blob_rw_data(blob)[:] = self.value
				self.post_msg(0, pmt.pmt_string_to_symbol("W"), blob)
			self.old_result = in0[x]
		self.consume(0, len(in0))
		return 0
			
