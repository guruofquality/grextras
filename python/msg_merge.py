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
#                   mod/demod with packets as i/o
# /////////////////////////////////////////////////////////////////////////////

class msg_merge(gr.block):
    """
    Wrap an arbitrary digital modulator in our packet handling framework.

    Send packets by calling send_pkt
    """
    def __init__(
        self,
    ):
        """
        The input is a pmt message blob.
        Non-blob messages will be ignored.
        The output is a byte stream for the modulator

        @param access_code: AKA sync vector
        @type access_code: string of 1's and 0's between 1 and 64 long
        @param use_whitener_offset: If true, start of whitener XOR string is incremented each packet
        """

		
        gr.block.__init__(
            self,
            name = "msg_merge",
            in_sig = [numpy.uint8, numpy.uint8, numpy.uint8],
            out_sig = [numpy.uint8],
            has_msg_input = False,
            num_msg_outputs = 0,
        )
    
        
    def work(self, input_items, output_items):
		in0 = input_items[0]
		nread = self.nitems_read(0) #number of items read on port 
		nread1 = self.nitems_read(1) #number of items read on port 
		nread2 = self.nitems_read(2) #number of items read on port 
		ninput_items = len(input_items[0])
		ninput_items = len(input_items[0])
		ninput_items = len(input_items[0])

		
		#read all tags associated with port 0 for items in this work function
		tags = self.get_tags_in_range(0, nread, nread+ninput_items)
		
		
		
		for tag in tags:
			self.add_item_tag(0, tag)
			
		tags = self.get_tags_in_range(1, nread, nread1+ninput_items)

		for tag in tags:
			self.add_item_tag(0, tag)
		
		tags = self.get_tags_in_range(2, nread, nread2+ninput_items)	
			
		for tag in tags:
			self.add_item_tag(0, tag)	
			
		self.consume(0, len(in0))
		return 0
