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


#arq definitions
ARQ_FALSE = 0
ARQ_TRUE = 1

# /////////////////////////////////////////////////////////////////////////////
#                   Virtual Channel Formatter
# /////////////////////////////////////////////////////////////////////////////

class virtual_channel_formatter(gr.block):
    """
    Provide ARQ capability, virtual channel capability, stat mux, etc
    """
    def __init__(
        self,channel,arq
    ):
        """
        Sets the key of a blob as neccassary for simple_mac and similar blocks.
        Encodes information like destination radio addr, ARQ settings etc.
        
        Blob in, blob out.

        @param destination_addr: physical address of radio the pkt is destined for
        @param ARQ: Use ARQ or don't
       """

        gr.block.__init__(
            self,
            name = "virtual_channel_formatter",
            in_sig = None,
            out_sig = None,
            num_msg_inputs = 1,
            num_msg_outputs = 1,
        )
    
        self.mgr = pmt.pmt_mgr()
        for i in range(64):
            self.mgr.set(pmt.pmt_make_blob(10000))
        
        self.channel = channel
        self.arq = arq

        
    def work(self, input_items, output_items):
        
        while(1):
            try: msg = self.pop_msg_queue()
            except: return -1

            if not pmt.pmt_is_blob(msg.value):
                continue

            blob = pmt.pmt_blob_data(msg.value)
            
            if self.arq == ARQ_TRUE:
                arq_char = 'U'
            else:
                arq_char = 'V'
                
            key_str = arq_char + chr(self.channel)
            
            self.post_msg(0, pmt.pmt_string_to_symbol(key_str), msg.value)
