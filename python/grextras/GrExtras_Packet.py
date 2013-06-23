#
# Copyright 1980-2013 Free Software Foundation, Inc.
# 
# This file is part of GrExtras
# 
# GrExtras is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3, or (at your option)
# any later version.
# 
# GrExtras is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with GrExtras; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street,
# Boston, MA 02110-1301, USA.
#

import ctypes
import numpy
import gras
import time
from PMC import *
from math import pi
try:
    import digital_swig as gr_digital
    import packet_utils
except ImportError:
    from gnuradio.digital import packet_utils
    import gnuradio.digital as gr_digital

from GrExtras_FramerSink import make_digital_framer_sink_1

# /////////////////////////////////////////////////////////////////////////////
#                   mod/demod with packets as i/o
# /////////////////////////////////////////////////////////////////////////////

class PacketFramer(gras.Block):
    """
    The input is a pmt message datagram.
    Non-datagram messages will be ignored.
    The output is a byte stream for the modulator
    """

    def __init__(
        self,
        samples_per_symbol,
        bits_per_symbol,
        access_code=None,
        use_whitener_offset=False,
        header_bytes=64,
        footer_bytes=64,
    ):
        """
        Create a new packet framer.
        @param access_code: AKA sync vector
        @type access_code: string of 1's and 0's between 1 and 64 long
        @param use_whitener_offset: If true, start of whitener XOR string is incremented each packet
        @param header_bytes: number of bytes to lead the packet to adapt for transients and AGC
        @param footer_bytes: number of bytes to trail the packet to flush through TX DSP filter
        """

        self._bits_per_symbol = bits_per_symbol
        self._samples_per_symbol = samples_per_symbol
        self._header = numpy.array([0x55] * header_bytes)
        self._footer = numpy.array([0x55] * footer_bytes)

        gras.Block.__init__(
            self,
            name = "GrExtras PacketFramer",
            in_sig = [numpy.uint8],
            out_sig = [numpy.uint8],
        )

        #setup output config for max packet MTU
        self.output_config(0).reserve_items = 4096 #max pkt frame possible

        self._use_whitener_offset = use_whitener_offset
        self._whitener_offset = 0

        if not access_code:
            access_code = packet_utils.default_access_code
        if not packet_utils.is_1_0_string(access_code):
            raise ValueError, "Invalid access_code %r. Must be string of 1's and 0's" % (access_code,)
        self._access_code = access_code

    def work(self, ins, outs):
        assert (len(ins[0]) == 0)

        msg = self.pop_input_msg(0)
        pkt_msg = msg()
        #print 'pop msg', msg, type(pkt_msg)
        if not isinstance(pkt_msg, gras.PacketMsg): return

        pkt = packet_utils.make_packet(
            pkt_msg.buff.get().tostring(),
            self._samples_per_symbol,
            self._bits_per_symbol,
            self._access_code,
            False, #pad_for_usrp,
            self._whitener_offset,
        )
        #print 'len pkt', len(pkt)
        pkt = numpy.fromstring(pkt, numpy.uint8)
        pkt = numpy.append(self._header, pkt)
        pkt = numpy.append(pkt, self._footer)

        if self._use_whitener_offset:
            self._whitener_offset = (self._whitener_offset + 1) % 16

        assert len(outs[0]) >= len(pkt)
        outs[0][:len(pkt)] = pkt
        self.produce(0, len(pkt))
        #print 'produce', len(pkt)

        #create an end of burst tag for each packet end
        length = ctypes.c_size_t(len(pkt))
        tag = gras.StreamTag(PMC_M("length"), PMC_M(length))
        self.post_output_tag(0, gras.Tag(self.get_produced(0), PMC_M(tag)))

        #post all tags found in the info
        if pkt_msg.info:
            try:
                for tag_p in pkt_msg.info():
                    tag = tag_p()
                    tag.offset += self.get_produced(0)
                    self.post_output_tag(0, tag)
            except: pass

class PacketDeframer(gras.HierBlock):
    """
    Hierarchical block for demodulating and deframing packets.

    The input is a byte stream from the demodulator.
    The output is a pmt message datagram.
    """

    def __init__(self, access_code=None, threshold=-1):
        """
        Create a new packet deframer.
        @param access_code: AKA sync vector
        @type access_code: string of 1's and 0's
        @param threshold: detect access_code with up to threshold bits wrong (-1 -> use default)
        @type threshold: int
        """

        gras.HierBlock.__init__(self, "PacketDeframer")

        if not access_code:
            access_code = packet_utils.default_access_code
        if not packet_utils.is_1_0_string(access_code):
            raise ValueError, "Invalid access_code %r. Must be string of 1's and 0's" % (access_code,)

        if threshold == -1:
            threshold = 12              # FIXME raise exception

        self.correlator = gr_digital.correlate_access_code_bb(access_code, threshold)
        self.framer_sink = make_digital_framer_sink_1()
        self._queue_to_datagram = _queue_to_datagram()
        self.connect(self, self.correlator, self.framer_sink, self._queue_to_datagram, self)





class _queue_to_datagram(gras.Block):
    """
    Helper for the deframer, reads queue, unpacks packets, posts.
    It would be nicer if the framer_sink output'd messages.
    """
    def __init__(self):
        gras.Block.__init__(
            self, name = "_queue_to_datagram",
            in_sig = [numpy.uint8], out_sig = [numpy.uint8],
        )

        #set the output reserve to the max expected pkt size
        self.output_config(0).reserve_items = 4096

    def work(self, ins, outs):
        assert (len(ins[0]) == 0)

        msg = self.pop_input_msg(0)
        pkt_msg = msg()
        #print 'pop msg', msg, type(pkt_msg)
        if not isinstance(pkt_msg, gras.PacketMsg): return
        whitener = pkt_msg.info()
        assert (isinstance(whitener, int))

        ok, payload = packet_utils.unmake_packet(pkt_msg.buff.get().tostring(), whitener)
        if ok:
            payload = numpy.fromstring(payload, numpy.uint8)

            #get a reference counted buffer to pass downstream
            buff = self.get_output_buffer(0)
            buff.offset = 0
            buff.length = len(payload)
            buff.get()[:] = numpy.fromstring(payload, numpy.uint8)

            self.post_output_msg(0, gras.PacketMsg(buff))
        else:
            print 'f',
            self.post_output_msg(0, gras.PacketMsg())
