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
#from numpy import np
from math import pi
from gnuradio import gr
from gruel import pmt
from gnuradio.digital import packet_utils
import gnuradio.digital as gr_digital
import block_gateway #needed to inject into gr
import Queue
import time



# /////////////////////////////////////////////////////////////////////////////
#                   Heart Beat - period blob generation with param key and value
# /////////////////////////////////////////////////////////////////////////////

class heart_beat(gr.block):
    """
    Generates blob at specified interval (w/ sleep)
    """
    def __init__(
        self,period,key,value
    ):
        """
        The input is a pmt message blob.
        Non-blob messages will be ignored.
        The output is a byte stream for the modulator

        @param period: Time between blopbs
        @param key: String for key
        @param value: String for value
        """

		
        gr.block.__init__(
            self,
            name = "simple_mac",
            in_sig = None,
            out_sig = None,
            has_msg_input = False,
            num_msg_outputs = 1,
        )
    
        self.mgr = pmt.pmt_mgr()
        for i in range(64):
			self.mgr.set(pmt.pmt_make_blob(10000))
        self.period = period
        self.key = key
        self.value = value
 
    def work(self, input_items, output_items):
		
		while(1):
			blob = self.mgr.acquire(True) #block
			pmt.pmt_blob_resize(blob, len(self.value))
			pmt.pmt_blob_rw_data(blob)[:] = numpy.fromstring(self.value,dtype="uint8")
			self.post_msg(0, pmt.pmt_string_to_symbol(self.key), blob)
			time.sleep(self.period)

						
