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

        try:
            import packet_utils
        except ImportError:
            from gnuradio.digital import packet_utils

        if not access_code:
            access_code = packet_utils.default_access_code
        if not packet_utils.is_1_0_string(access_code):
            raise ValueError, "Invalid access_code %r. Must be string of 1's and 0's" % (access_code,)

        if threshold == -1:
            threshold = 12              # FIXME raise exception

        try:
            import digital_swig as gr_digital
        except ImportError:
            import gnuradio.digital as gr_digital
        self.correlator = gr_digital.correlate_access_code_bb(access_code, threshold)
        self.framer_sink = gras.make('/extras/framer_sink_1')
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

        try:
            import packet_utils
        except ImportError:
            from gnuradio.digital import packet_utils
        self.packet_utils = packet_utils

    def work(self, ins, outs):
        assert (len(ins[0]) == 0)

        msg = self.pop_input_msg(0)
        pkt_msg = msg()
        #print 'pop msg', msg, type(pkt_msg)
        if not isinstance(pkt_msg, gras.PacketMsg): return
        whitener = pkt_msg.info()
        assert (isinstance(whitener, int))

        ok, payload = self.packet_utils.unmake_packet(pkt_msg.buff.get().tostring(), whitener)
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

gras.register_factory("/extras/packet_deframer", PacketDeframer)
