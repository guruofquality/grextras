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
import time
import extras_swig 

SEARCH_EOB_IN = 0
FOUND_EOB_IN = 1

# /////////////////////////////////////////////////////////////////////////////
#                   Burst Gate - moves EOB to end of sample set
#                   Useful for DSP chains with propgation after eob insertion
# /////////////////////////////////////////////////////////////////////////////

class burst_gate(gr.block):
    """
    Moves EOB to the end of a set of samples.
    """
    def __init__(self):
        gr.block.__init__(self, name="burst_gate", in_sig=[numpy.complex64], out_sig=[numpy.complex64])
        #self.set_auto_consume(False)
        self.set_tag_propagation_policy(extras_swig.TPP_DONT)
    

    def work(self, input_items, output_items):
        self.state = SEARCH_EOB_IN
        
        in0 = input_items[0]
        out = output_items[0]
        
        out[:] = in0[:]

        nread = self.nitems_read(0) #number of items read on port 0
        ninput_items = len(input_items[0])

        #read all tags associated with port 0 for items in this work function
        tags = self.get_tags_in_range(0, nread, nread+ninput_items)
        
        num_items = min(len(input_items[0]), len(output_items[0]))
        
        if (len(input_items[0]) > len(output_items[0])):
            print "Burst Gate: Output items small. Might be bad."
 
        source = pmt.pmt_string_to_symbol("")
        
        for tag in tags:
            if pmt.pmt_symbol_to_string(tag.key) == "tx_eob":
                self.state = FOUND_EOB_IN
            else:
                self.add_item_tag(0, tag.offset, tag.key, tag.value, source)

        if self.state == FOUND_EOB_IN:
            item_index = num_items #which output item gets the tag?
            offset = self.nitems_written(0) + item_index
            key = pmt.pmt_string_to_symbol("tx_eob")
            
            value = pmt.pmt_string_to_symbol("")
            source = pmt.pmt_string_to_symbol("")
            self.add_item_tag(0, offset - 1, key, pmt.PMT_T, source)

        return len(out)
