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

OTA_OUT = 'U'
USER_DATA = 'D'
USER_DATA_MULTIPLEXED = 'E'

OTA_IN = 'V'
INTERNAL = 'W'

HEARTBEAT = 'H'


ARQ_REQ = 85
ARQ_NO_REQ = 86

ARQ_PROTOCOL_ID = 90
BROADCAST_PROTOCOL_ID = 91
USER_IO_PROTOCOL_ID = 92
USER_IO_MULTIPLEXED_ID = 93

BROADCAST_ADDR = 255

#block port definitions
RADIO_PORT = 0
APP_PORT = 1
CTRL_PORT = 2

#msg key indexes for radio outbound blobs
KEY_INDEX_CTRL = 0
KEY_INDEX_DEST_ADDR = 1

#msg key indexes for internal control messages
KEY_INT_MSG_TYPE = 0    

#Packet index definitions
PKT_INDEX_CTRL = 4
PKT_INDEX_PROT_ID = 3     
PKT_INDEX_DEST = 2
PKT_INDEX_SRC = 1
PKT_INDEX_CNT = 0

#ARQ Channel States
ARQ_CHANNEL_BUSY = 1
ARQ_CHANNEL_IDLE = 0

# /////////////////////////////////////////////////////////////////////////////
#                   Simple MAC w/ ARQ
# /////////////////////////////////////////////////////////////////////////////

class simple_mac(gr.block):
    """
    Provide ARQ capability, virtual channel capability, stat mux, etc
    """
    def __init__(
        self,addr,timeout,max_attempts
    ):
        """
        three inputs: radio, from_app, ctrl_in
        Three outputs: radio, to_app, ctrl_out

        """

        
        gr.block.__init__(
            self,
            name = "simple_mac",
            in_sig = None,
            out_sig = None,
            num_msg_inputs = 3,
            num_msg_outputs = 3,
        )
    
        self.mgr = pmt.pmt_mgr()
        for i in range(64):
            self.mgr.set(pmt.pmt_make_blob(10000))
        
        self.addr = addr                                #MAC's address
        
        self.pkt_cnt_arq = 0                            #pkt_cnt for arq channel
        self.pkt_cnt_no_arq = 0                            #pkt_cnt for non_arq channel
        
        self.arq_expected_sequence_number = 0            #keep track for sequence error detection
        self.no_arq_expected_sequence_number = 0        #keep track for sequence error detection

        self.arq_sequence_error_cnt = 0                    #arq channel seq errors - VERY BAD
        self.no_arq_sequence_error_cnt = 0                #non-arq channel seq error count
        self.arq_pkts_txed = 0                            #how many arq packets we've transmitted
        self.arq_retxed = 0                                #how many times we've retransmitted
        self.failed_arq = 0
        self.max_attempts = max_attempts
                                
        self.arq_channel_state = ARQ_CHANNEL_IDLE
        self.expected_arq_id = 0                        #arq id we're expected to get ack for      
        self.timeout = timeout                            #arq timeout parameter
        self.time_of_tx = 0.0                            #time of last arq transmission
        
        self.queue = Queue.Queue()                        #queue for msg destined for ARQ path
    
    #transmit data - msg is numpy array
    def send_pkt_radio(self,msg,pkt_cnt,dest,protocol_id,control):
        pkt_str = chr(pkt_cnt) + chr(self.addr) + chr(dest) + chr(protocol_id) + chr(control) + pmt.pmt_blob_data(msg.value).tostring()
        blob = self.mgr.acquire(True) #block
        pmt.pmt_blob_resize(blob, len(pkt_str))
        pmt.pmt_blob_rw_data(blob)[:] = numpy.fromstring(pkt_str, dtype='uint8')
        self.post_msg(0, pmt.pmt_string_to_symbol('U'), blob)
        return

    #transmit data - msg is string
    def send_pkt_radio_2(self,msg,pkt_cnt,dest,protocol_id,control):
        pkt_str = chr(pkt_cnt) + chr(self.addr) + chr(dest) + chr(protocol_id) + chr(control) + msg
        blob = self.mgr.acquire(True) #block
        pmt.pmt_blob_resize(blob, len(pkt_str))
        pmt.pmt_blob_rw_data(blob)[:] = numpy.fromstring(pkt_str, dtype='uint8')
        self.post_msg(0, pmt.pmt_string_to_symbol('U'), blob)
        return    
        
    #transmit data through non-arq path    
    def tx_no_arq(self,msg,dest,protocol_id):
        self.send_pkt_radio(msg,self.pkt_cnt_no_arq,dest,protocol_id,ARQ_NO_REQ)
        self.pkt_cnt_no_arq = ( self.pkt_cnt_no_arq + 1 ) % 255
        return
    
    #transmit data through arq path
    def tx_arq(self,msg,dest,protocol_id):
        self.send_pkt_radio(msg,self.pkt_cnt_arq,dest,protocol_id,ARQ_REQ)
        #self.pkt_cnt_arq = ( self.pkt_cnt_arq + 1 ) % 255
        #self.queue.put(msg)
        return
      
    #transmit ack packet
    #TODO: make this a more generice call to tx_no_arq 
    #TODO: kill send_pkt_radio_2, or send_pkt_radio 
    def send_ack(self,msg,ack_addr,ack_pkt_cnt):
       msg = chr(ack_pkt_cnt)
       self.send_pkt_radio_2(msg,self.pkt_cnt_no_arq,ack_addr,ARQ_PROTOCOL_ID,ARQ_NO_REQ)
       self.pkt_cnt_no_arq = ( self.pkt_cnt_no_arq + 1 ) % 255

       
    #transmit layer 3 broadcast packet
    def send_bcast_pkt(self):
        msg = ''
        self.send_pkt_radio_2(msg,BROADCAST_ADDR,BROADCAST_PROTOCOL_ID,ARQ_NO_REQ)
    
    #output user data received over the air
    def output_user_data(self,pkt):
        pkt_len = len(pkt)        
        if (pkt_len > 5):
            payload = pkt[5:]
        blob = self.mgr.acquire(True) #block
        pmt.pmt_blob_resize(blob, len(payload))
        pmt.pmt_blob_rw_data(blob)[:] = payload
        self.post_msg(1, pmt.pmt_string_to_symbol('U'), blob)

            
    def work(self, input_items, output_items):
        
        while(1):
            try: msg = self.pop_msg_queue()
            except: return -1
            
            if not pmt.pmt_is_blob(msg.value): 
                print self.addr
                print "not a blob - simple mac"
                continue

            self.key_string = pmt.pmt_symbol_to_string(msg.key)

            #receive data from application port to be transmitter to radio
            if msg.offset == APP_PORT:
                if(ord(self.key_string[KEY_INDEX_CTRL]) == ARQ_REQ):
                    self.queue.put(msg)
                if(ord(self.key_string[KEY_INDEX_CTRL]) == ARQ_NO_REQ):
                    self.tx_no_arq(msg,ord(self.key_string[KEY_INDEX_DEST_ADDR]),USER_IO_PROTOCOL_ID)        
            
            #TODO: Oh my goodness, this section is ugly. I need to clean this.
            #process the packet    received on radio    
            if(msg.offset == RADIO_PORT):
                incoming_pkt = pmt.pmt_blob_data(msg.value)    #get data
                if ( len(incoming_pkt) > 5 ): #check for weird header only stuff
                    if( ( incoming_pkt[PKT_INDEX_DEST] == self.addr or incoming_pkt[PKT_INDEX_DEST] == 255)  and not incoming_pkt[PKT_INDEX_SRC] == self.addr):    #for us?         
                        #check to see if we must ACK this packet
                        if(incoming_pkt[PKT_INDEX_CTRL] == ARQ_REQ): #TODO, stuff CTRL and Protocol in one field
                            self.send_ack(incoming_pkt,incoming_pkt[PKT_INDEX_SRC],incoming_pkt[PKT_INDEX_CNT])                        #Then send ACK then
                            if not (self.arq_expected_sequence_number == incoming_pkt[PKT_INDEX_CNT]):
                                self.arq_sequence_error_cnt += 1
                            self.arq_expected_sequence_number =  ( incoming_pkt[PKT_INDEX_CNT] + 1 ) % 255 
                        else:
                            if not (self.no_arq_expected_sequence_number == incoming_pkt[PKT_INDEX_CNT]):
                                self.no_arq_sequence_error_cnt += 1
                                #print self.no_arq_sequence_error_cnt
                                #print self.no_arq_sequence_error_cnt,incoming_pkt[PKT_INDEX_CNT],self.no_arq_expected_sequence_number
                            self.no_arq_expected_sequence_number =  ( incoming_pkt[PKT_INDEX_CNT] + 1 ) % 255 

                        incoming_protocol_id = incoming_pkt[PKT_INDEX_PROT_ID]
                        
                        #check to see if this is an ACK packet
                        if(incoming_protocol_id == ARQ_PROTOCOL_ID):
                            if incoming_pkt[5] == self.expected_arq_id:
                                self.arq_channel_state = ARQ_CHANNEL_IDLE
                                self.pkt_cnt_arq = ( self.pkt_cnt_arq + 1 ) % 255
                            else:
                                print 'received out of sequence ack',incoming_pkt[5],self.expected_arq_id
                        
                        #do something with incoming user data
                        elif(incoming_protocol_id == USER_IO_PROTOCOL_ID):
                            self.output_user_data(incoming_pkt)                        
                                
                        else:
                            print 'unknown protocol'

            if(msg.offset == CTRL_PORT):
                if(self.key_string[KEY_INT_MSG_TYPE] == HEARTBEAT):
                        #TODO: something useful when we receive heartbeat?
                        a = '0'
                
                        
                                    
            #check to see if we have any outgoing messages from arq buffer we should send
            #or pending re-transmissions
            if self.arq_channel_state == ARQ_CHANNEL_IDLE: #channel ready for next arq msg
                if not self.queue.empty(): #we have an arq msg to send, so lets send it
                    #print self.queue.qsize()
                    self.outgoing_msg = self.queue.get() #get msg
                    self.expected_arq_id = self.pkt_cnt_arq #store it for re-use
                    self.key_string = pmt.pmt_symbol_to_string(self.outgoing_msg.key) #pull key
                    self.tx_arq(self.outgoing_msg,ord(self.key_string[KEY_INDEX_DEST_ADDR]),USER_IO_PROTOCOL_ID) #transmit it
                    self.time_of_tx = time.time() # note time for arq timeout recognition
                    self.arq_channel_state = ARQ_CHANNEL_BUSY #remember that the channel is busy
                    self.arq_pkts_txed += 1
                    self.retries = 0
            else: #if channel is busy, lets check to see if its time to re-transmit
                if ( time.time() - self.time_of_tx ) > self.timeout: #check for ack timeout
                    if self.retries == self.max_attempts:            #know when to quit
                        self.retries = 0 
                        self.arq_channel_state = ARQ_CHANNEL_IDLE
                        self.failed_arq += 1
                        self.pkt_cnt_arq = ( self.pkt_cnt_arq + 1 ) % 255   #start on next pkt
                        print 'pkt failed arq'
                    else:    
                        self.key_string = pmt.pmt_symbol_to_string(self.outgoing_msg.key) #reset key
                        self.tx_arq(self.outgoing_msg,ord(self.key_string[KEY_INDEX_DEST_ADDR]),USER_IO_PROTOCOL_ID) #tx again
                        self.time_of_tx = time.time()
                        self.arq_retxed += 1
                        self.retries += 1
                        #TODO: implement exponential back-off
                    
