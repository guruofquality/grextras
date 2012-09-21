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


OTA_OUT = 'U'
OTA_IN = 'V'
INTERNAL = 'W'

READY = 0 
NOT_READY = 1

# /////////////////////////////////////////////////////////////////////////////
#                   Channel Access Controller
# /////////////////////////////////////////////////////////////////////////////

class channel_access_controller(gr.block):
    """
    Wrap an arbitrary digital modulator in our packet handling framework.

    Send packets by calling send_pkt
    """
    def __init__(
        self
    ):
        """
        First input is a state.
        Will hold state staticly until updated received on first input
        Will queue messages from second input.
        Will pass queued messages to output when state is '0'
       """

		
        gr.block.__init__(
            self,
            name = "channel_access_controller",
            in_sig = None,
            out_sig = None,
            num_msg_inputs = 2,
            num_msg_outputs = 1,
        )
    
        self.mgr = pmt.pmt_mgr()
        for i in range(64):
			self.mgr.set(pmt.pmt_make_blob(10000))
        
        self.q = Queue.Queue(maxsize=0)
        self.channel_state = READY
 		    
    def work(self, input_items, output_items):
		
		while(1):
			try: msg = self.pop_msg_queue()
			except: return -1
			
			if not pmt.pmt_is_blob(msg.value): 
				print self.addr
				print "not a blob - simple mac"
				continue
			
			self.key_string = pmt.pmt_symbol_to_string(msg.key)
			self.value = pmt.pmt_blob_data(msg.value)
            
            
			if(msg.offset == 0):
				if (len(self.value) > 0):
					if(self.value[0] == 0):
						self.channel_state = READY
					else:
						self.channel_state = NOT_READY
		
			if(msg.offset == 1):
				self.q.put(msg)
				if(self.channel_state == NOT_READY): print 'hold back'
				
			if (self.channel_state == READY):
				while not self.q.empty():
					self.q.get()
					self.post_msg(0, msg)
