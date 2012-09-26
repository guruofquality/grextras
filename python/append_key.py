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
#                   Blob Append Key (overwrites)
# /////////////////////////////////////////////////////////////////////////////

class append_key(gr.block):
    """
    Block over-writes key of a blob.
    
    """
    #TODO: add support for prepend, append modes
    def __init__(
        self,key
    ):
        """
        The input is a pmt message blob.
        Non-blob messages will be ignored.
        The output is pmt message blob

        @param key: String replacement for key
        """
        gr.block.__init__(
            self,
            name = "append_key",
            in_sig = None,
            out_sig = None,
            has_msg_input = True,
            num_msg_outputs = 1,
            
        )
    
        self.key = key


        self.mgr = pmt.pmt_mgr()
        for i in range(64):
			self.mgr.set(pmt.pmt_make_blob(10000))
        
 
    def work(self, input_items, output_items):
		
		while(1):
			try: msg = self.pop_msg_queue()
			except: return -1
			
			if not pmt.pmt_is_blob(msg.value): 
				continue

			self.post_msg(0, pmt.pmt_string_to_symbol(self.key), msg.value)
	  

