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
from gruel import pmt

# /////////////////////////////////////////////////////////////////////////////
#                  msg_to_stdout.py - prints 
# /////////////////////////////////////////////////////////////////////////////

class msg_to_stdout(gr.block):
    """
    Simply prints the key and value of a blob or general pmt to stdout
    Useful for debugging msg based applications.
    """
    def __init__(
        self,
    ):
        """
        The input is a pmt message blob.
        No outputs.
        Prints contents of blobs.
        """

        
        gr.block.__init__(
            self,
            name = "msg_to_stdout",
            in_sig = None,
            out_sig = None,
            has_msg_input = True,
            num_msg_outputs = 0,            
        )
  
 
    def work(self, input_items, output_items):
        
        while(1):
            try: msg = self.pop_msg_queue()
            except: return -1
            
            print  
    
            if not pmt.pmt_is_blob(msg.value): 
                print "BTW, this not a blob"
                print "Key: ",pmt.pmt_symbol_to_string(msg.key),"Value: ",pmt.pmt_symbol_to_string(msg.key)

            else:
                print "Key: ",pmt.pmt_symbol_to_string(msg.key),"Value: ",pmt.pmt_blob_data(msg.value).tostring()                
   

            
