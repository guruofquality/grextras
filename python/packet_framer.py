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

class packet_framer(gr.block):
    """
    The input is a pmt message blob.
    Non-blob messages will be ignored.
    The output is a byte stream for the modulator
    """

    def __init__(
        self,
        samples_per_symbol,
        bits_per_symbol,
        access_code=None,
        use_whitener_offset=False
    ):
        """
        Create a new packet framer.
        @param access_code: AKA sync vector
        @type access_code: string of 1's and 0's between 1 and 64 long
        @param use_whitener_offset: If true, start of whitener XOR string is incremented each packet
        """

        self._bits_per_symbol = bits_per_symbol
        self._samples_per_symbol = samples_per_symbol

        gr.block.__init__(
            self,
            name = "mod_pkts2",
            in_sig = None,
            out_sig = [numpy.uint8],
            has_msg_input = True,
        )

        self._use_whitener_offset = use_whitener_offset
        self._whitener_offset = 0

        if not access_code:
            access_code = packet_utils.default_access_code
        if not packet_utils.is_1_0_string(access_code):
            raise ValueError, "Invalid access_code %r. Must be string of 1's and 0's" % (access_code,)
        self._access_code = access_code
        self._pkt = []
        self.more_frame_cnt = 0
        self.keep = False

    def work(self, input_items, output_items):
        while not len(self._pkt):
            try: msg = self.pop_msg_queue()
            except: return -1
            if not pmt.pmt_is_blob(msg.value): 
                self.tx_time,data,self.more_frame_cnt = pmt.to_python(msg.value)
                self.has_tx_time = True
                #print data
                #print tx_time
                #print data.tostring()
            else:
                data = pmt.pmt_blob_data(msg.value)
                #print data
                self.has_tx_time = False
            
                
            pkt = packet_utils.make_packet(
                data.tostring(),
                self._samples_per_symbol,
                self._bits_per_symbol,
                self._access_code,
                False, #pad_for_usrp,
                self._whitener_offset,
                )
            self._pkt = numpy.fromstring(pkt, numpy.uint8)
            if self._use_whitener_offset:
                self._whitener_offset = (self._whitener_offset + 1) % 16

            #shouldn't really need to send start of burst
            #only need to do sob if looking for timed transactions

            num_items = min(len(self._pkt), len(output_items[0]))
            output_items[0][:num_items] = self._pkt[:num_items]
            self._pkt = self._pkt[num_items:] #residue for next work()
            
            if len(self._pkt) == 0 :
                item_index = num_items #which output item gets the tag?
                offset = self.nitems_written(0) + item_index
                source = pmt.pmt_string_to_symbol("framer")
                
                #print 'frame cnt',self.more_frame_cnt
                
                if self.has_tx_time:
                    key = pmt.pmt_string_to_symbol("tx_sob")
                    self.add_item_tag(0, self.nitems_written(0), key, pmt.PMT_T, source)
                    key = pmt.pmt_string_to_symbol("tx_time")
                    self.add_item_tag(0, self.nitems_written(0), key, pmt.from_python(self.tx_time), source)
                    #if self.keep:
                    #    print 'bad order'
                    #self.keep = True

                
                if self.more_frame_cnt == 0:
                    key = pmt.pmt_string_to_symbol("tx_eob")
                    self.add_item_tag(0, offset - 1, key, pmt.PMT_T, source)
                    #if self.keep:
                    #    print 'good order'
                    #self.keep = False
                else:
                    self.more_frame_cnt -= 1

 
                
            return num_items
        
class packet_deframer(gr.hier_block2):
    """
    Hierarchical block for demodulating and deframing packets.

    The input is a byte stream from the demodulator.
    The output is a pmt message blob.
    """

    def __init__(self, access_code=None, threshold=-1):
        """
        Create a new packet deframer.
        @param access_code: AKA sync vector
        @type access_code: string of 1's and 0's
        @param threshold: detect access_code with up to threshold bits wrong (-1 -> use default)
        @type threshold: int
        """

        gr.hier_block2.__init__(
            self,
            "demod_pkts2",
            gr.io_signature(1, 1, 1),
            gr.io_signature(1, 1, 1),
        )

        if not access_code:
            access_code = packet_utils.default_access_code
        if not packet_utils.is_1_0_string(access_code):
            raise ValueError, "Invalid access_code %r. Must be string of 1's and 0's" % (access_code,)

        if threshold == -1:
            threshold = 12              # FIXME raise exception

        msgq = gr.msg_queue(4)          # holds packets from the PHY
        self.correlator = gr_digital.correlate_access_code_bb(access_code, threshold)

        self.framer_sink = gr.framer_sink_1(msgq)
        self.connect(self, self.correlator, self.framer_sink)
        self._queue_to_blob = _queue_to_blob(msgq)
        self.connect(self._queue_to_blob, self)





class _queue_to_blob(gr.block):
    """
    Helper for the deframer, reads queue, unpacks packets, posts.
    It would be nicer if the framer_sink output'd messages.
    """
    def __init__(self, msgq):
        gr.block.__init__(
            self, name = "_queue_to_blob",
            in_sig = None, out_sig = None,
            num_msg_outputs = 1
        )
        self._msgq = msgq
        self._mgr = pmt.pmt_mgr()
        for i in range(64):
            self._mgr.set(pmt.pmt_make_blob(10000))

    def work(self, input_items, output_items):
        while True:
            try: msg = self._msgq.delete_head()
            except: return -1
            ok, payload = packet_utils.unmake_packet(msg.to_string(), int(msg.arg1()))
            if ok:
                payload = numpy.fromstring(payload, numpy.uint8)
                try: blob = self._mgr.acquire(True) #block
                except: return -1
                pmt.pmt_blob_resize(blob, len(payload))
                pmt.pmt_blob_rw_data(blob)[:] = payload
                self.post_msg(0, pmt.pmt_string_to_symbol("ok"), blob)
            else:
                a = 0

