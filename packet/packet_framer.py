#
# Copyright 1980-2013 Free Software Foundation, Inc.
# Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.
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

        try:
            import packet_utils
        except ImportError:
            from gnuradio.digital import packet_utils
        self.packet_utils = packet_utils

        if not access_code:
            access_code = self.packet_utils.default_access_code
        if not self.packet_utils.is_1_0_string(access_code):
            raise ValueError, "Invalid access_code %r. Must be string of 1's and 0's" % (access_code,)
        self._access_code = access_code

    def work(self, ins, outs):
        assert (len(ins[0]) == 0)

        msg = self.pop_input_msg(0)
        pkt_msg = msg()
        #print 'pop msg', msg, type(pkt_msg)
        if not isinstance(pkt_msg, gras.PacketMsg): return

        pkt = self.packet_utils.make_packet(
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

gras.register_factory("/extras/packet_framer", PacketFramer)
