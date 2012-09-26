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


BROADCAST_ADDR = 255

#block port definitions - inputs
OUTGOING_PKT_PORT = 0
INCOMING_PKT_PORT = 1
CTRL_PORT = 2

#block port definitions - outputs
TO_FRAMER_PORT = 0
CTRL_PORT = 1

#Time state machine
LOOKING_FOR_TIME = 0 
HAVE_TIME = 0


# /////////////////////////////////////////////////////////////////////////////
#                   TDMA MAC
# /////////////////////////////////////////////////////////////////////////////

class tdma_engine(gr.block):
    """
    TDMA implementation.  See wiki for more details
    """
    def __init__(
        self,initial_slot,slot_interval,guard_interval,num_slots,lead_limit,link_bps
    ):
        """
        Inputs: complex stream from USRP, pkt in, ctrl in
        Outputs: pkt out, ctrl out
        """

        gr.block.__init__(
            self,
            name = "tdma_engine",
            in_sig = [numpy.complex64],
            out_sig = None,
            num_msg_inputs = 3,
            num_msg_outputs = 2,
        )
    
        self.mgr = pmt.pmt_mgr()
        for i in range(64):
            self.mgr.set(pmt.pmt_make_blob(10000))
        
        self.initial_slot = initial_slot
        self.slot_interval = slot_interval
        self.guard_interval = guard_interval
        self.num_slots = num_slots
        self.lead_limit = lead_limit
        self.link_bps = link_bps
        
        self.bytes_per_slot = int( ( self.slot_interval - self.guard_interval ) * self.link_bps / 8 )
        
        self.queue = Queue.Queue()                        #queue for msg destined for ARQ path
        self.tx_queue = Queue.Queue()
        
        self.last_rx_time = 0
        self.last_rx_rate = 0
        self.samples_since_last_rx_time = 0
        
        self.next_interval_start = 0
        self.next_transmit_start = 0

        self.know_time = False
        self.found_time = False
        self.found_rate = False
        self.set_tag_propagation_policy(extras_swig.TPP_DONT)

        self.has_old_msg = False
        self.overhead = 15
        self.pad_data = numpy.fromstring('this idsaf;lkjkfdjsd;lfjs;lkajskljf;klajdsfk',dtype='uint8')
        self.tx_slots_passed = 0
    
    def tx_frames(self):
        #send_sob
        #self.post_msg(TO_FRAMER_PORT, pmt.pmt_string_to_symbol('tx_sob'), pmt.PMT_T, pmt.pmt_string_to_symbol('tx_sob'))

        #get all of the packets we want to send
        total_byte_count = 0
        frame_count = 0
        
        #put residue from previous execution
        if self.has_old_msg:
            length = len(pmt.pmt_blob_data(self.old_msg.value)) + self.overhead
            total_byte_count += length
            self.tx_queue.put(self.old_msg)
            frame_count += 1
            self.has_old_msg = False
            print 'old msg'

        #fill outgoing queue until empty or maximum bytes queued for slot
        while(not self.queue.empty()):
            msg = self.queue.get()
            length = len(pmt.pmt_blob_data(msg.value)) + self.overhead
            total_byte_count += length
            if total_byte_count >= self.bytes_per_slot:
                self.has_old_msg = True
                self.old_msg = msg
                print 'residue'
                continue
            else:
                self.has_old_msg = False
                self.tx_queue.put(msg)
                frame_count += 1
        
        time_object = int(math.floor(self.antenna_start)),(self.antenna_start % 1)
        
        #if no data, send a single pad frame
        #TODO: add useful pad data, i.e. current time of SDR
        if frame_count == 0:
            data = self.pad_data
            more_frames = 0
            tx_object = time_object,data,more_frames
            self.post_msg(TO_FRAMER_PORT,pmt.pmt_string_to_symbol('full'),pmt.from_python(tx_object),pmt.pmt_string_to_symbol('tdma'))
        else:
            #print frame_count,self.queue.qsize(), self.tx_queue.qsize()
            #send first frame w tuple for tx_time and number of frames to put in slot
            blob = self.mgr.acquire(True) #block
            more_frames = frame_count - 1
            msg = self.tx_queue.get()
            data = pmt.pmt_blob_data(msg.value)
            tx_object = time_object,data,more_frames
            self.post_msg(TO_FRAMER_PORT,pmt.pmt_string_to_symbol('full'),pmt.from_python(tx_object),pmt.pmt_string_to_symbol('tdma'))
            frame_count -= 1
            
            
            old_data = []
            #print 'frame count: ',frame_count
            #send remining frames, blob only
            while(frame_count > 0):
                msg = self.tx_queue.get()
                data = pmt.pmt_blob_data(msg.value)
                blob = self.mgr.acquire(True) #block
                pmt.pmt_blob_resize(blob, len(data))
                pmt.pmt_blob_rw_data(blob)[:] = data
                self.post_msg(TO_FRAMER_PORT,pmt.pmt_string_to_symbol('d_only'),blob,pmt.pmt_string_to_symbol('tdma'))
                frame_count -= 1
        
        #print total_byte_count
        
    def work(self, input_items, output_items):
        
        #check for msg inputs when work function is called
        if self.check_msg_queue():
            try: msg = self.pop_msg_queue()
            except: return -1

            if msg.offset == OUTGOING_PKT_PORT:
                self.queue.put(msg)                 #if outgoing, put in queue for processing
            elif msg.offset == INCOMING_PKT_PORT:
                a = 0                               #TODO:something intelligent for incoming time bcast pkts
            else:
                a = 0                               #CONTROL port
            
        #process streaming samples and tags here
        in0 = input_items[0]
        nread = self.nitems_read(0) #number of items read on port 0
        ninput_items = len(input_items[0])

        #read all tags associated with port 0 for items in this work function
        tags = self.get_tags_in_range(0, nread, nread+ninput_items)

        #lets find all of our tags, making the appropriate adjustments to our timing
        for tag in tags:
            key_string = pmt.pmt_symbol_to_string(tag.key)
            if key_string == "rx_time":
                self.samples_since_last_rx_time = 0
                self.current_integer,self.current_fractional = pmt.to_python(tag.value)
                self.time_update = self.current_integer + self.current_fractional
                self.found_time = True
            elif key_string == "rx_rate":
                self.rate = pmt.to_python(tag.value)
                self.sample_period = 1/self.rate
                self.found_rate = True
        
        #determine first transmit slot when we learn the time
        if not self.know_time:
            if self.found_time and self.found_rate:
                self.know_time = True
                self.frame_period = self.slot_interval * self.num_slots
                my_fraction_frame = ( self.initial_slot * 1.0 ) / ( self.num_slots)
                frame_count = math.floor(self.time_update / self.frame_period)
                current_slot_interval = ( self.time_update % self.frame_period ) / self.frame_period
                self.time_transmit_start = (frame_count + 2) * self.frame_period + ( my_fraction_frame * self.frame_period ) - self.lead_limit
        
        #get current time
        self.time_update += (self.sample_period * ninput_items)

        #determine if it's time for us to start tx'ing, start process self.lead_limit seconds
        #before our slot actually begins (i.e. deal with latency)
        if self.time_update > self.time_transmit_start:
            self.interval_start = self.time_transmit_start + self.lead_limit
            self.antenna_start = self.interval_start + self.guard_interval
            self.tx_frames()  #do more than this?
            self.time_transmit_start += self.frame_period
            #TODO: add intelligence to handle slot changes safely
            
        return ninput_items
