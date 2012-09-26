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
import Queue
import time
import math
import extras_swig 



# /////////////////////////////////////////////////////////////////////////////
#                   Virtual Channel Formatter
# /////////////////////////////////////////////////////////////////////////////

class virtual_channel_mux(gr.block):
    """
    Provide ARQ capability, virtual channel capability, stat mux, etc
    """
    def __init__(
        self,ports
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
            name = "virtual_channel_mux",
            in_sig = None,
            out_sig = None,
            num_msg_inputs = ports,
            num_msg_outputs = 1,
        )
    
        self.mgr = pmt.pmt_mgr()
        for i in range(64):
            self.mgr.set(pmt.pmt_make_blob(10000))
        
    def work(self, input_items, output_items):
        
        while(1):
            try: msg = self.pop_msg_queue()
            except: return -1

            print 'got a message, now what?'
