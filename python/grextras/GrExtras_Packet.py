#
# Copyright 1980-2012 Free Software Foundation, Inc.
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

import numpy
import gras
import time
from PMC import *
from math import pi
from gnuradio import gr
try:
    import digital_swig as gr_digital
    import packet_utils
except ImportError:
    from gnuradio.digital import packet_utils
    import gnuradio.digital as gr_digital

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

        gras.Block.__init__(
            self,
            name = "GrExtras PacketFramer",
            in_sig = [numpy.uint8],
            out_sig = [numpy.uint8],
        )

        self._use_whitener_offset = use_whitener_offset
        self._whitener_offset = 0

        config = self.get_input_config(0)
        config.reserve_items = 0
        self.set_input_config(0, config)

        if not access_code:
            access_code = packet_utils.default_access_code
        if not packet_utils.is_1_0_string(access_code):
            raise ValueError, "Invalid access_code %r. Must be string of 1's and 0's" % (access_code,)
        self._access_code = access_code

        self._pkts = numpy.array([], numpy.uint8)

    def work(self, ins, outs):
        self.consume(0, len(ins[0]))
        print 'pop msg'
        msg = self.pop_input_msg(0)
        if msg.key == "datagram":
            #if not msg.key: return
            #if msg.key != "datagram": return
            #if not isinstance(msg.value, gras.SBuffer): return
            print 'msg.value.use_count()', msg.value.use_count()
            print 'msg.value.offset', msg.value.offset
            print 'msg.value.length', msg.value.length
            print 'packet_utils go'
            pkt = packet_utils.make_packet(
                msg.value.get().tostring(),
                self._samples_per_symbol,
                self._bits_per_symbol,
                self._access_code,
                False, #pad_for_usrp,
                self._whitener_offset,
            )
            print 'packet_utils done'
            #print 'len buff', t.value.length
            #print 'len pkt', len(pkt)
            self._pkts = numpy.append(self._pkts, numpy.fromstring(pkt, numpy.uint8))

            if self._use_whitener_offset:
                self._whitener_offset = (self._whitener_offset + 1) % 16

        if not len(self._pkts): return

        n = min(len(outs[0]), len(self._pkts))
        outs[0][:n] = self._pkts[:n]
        self._pkts = self._pkts[n:]
        self.produce(0, n)
        #print 'produce', n

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

        msgq = gr.msg_queue(4)          # holds packets from the PHY
        self.correlator = gr_digital.correlate_access_code_bb(access_code, threshold)

        self.framer_sink = gr_digital.framer_sink_1(msgq)
        self.connect(self, self.correlator, self.framer_sink)
        self._queue_to_datagram = _queue_to_datagram(msgq)
        self.connect(self._queue_to_datagram, self)





class _queue_to_datagram(gras.Block):
    """
    Helper for the deframer, reads queue, unpacks packets, posts.
    It would be nicer if the framer_sink output'd messages.
    """
    def __init__(self, msgq):
        gras.Block.__init__(
            self, name = "_queue_to_datagram",
            in_sig = None, out_sig = [numpy.uint8],
        )
        self._msgq = msgq

        #set the output reserve to the max expected pkt size
        config = self.get_output_config(0)
        config.reserve_items = 4096
        self.set_output_config(0, config)

        self.x = 0

        #we are going to block in work on a interruptible call
        self.set_interruptible_work(True)

    def work(self, ins, outs):
        #print '_queue_to_datagram work'
        try: msg = self._msgq.delete_head()
        except Exception:
            print 'staph!!'
            return
        ok, payload = packet_utils.unmake_packet(msg.to_string(), int(msg.arg1()))
        print 'got a msg', self.x, len(payload)
        self.x +=1
        if ok:
            payload = numpy.fromstring(payload, numpy.uint8)

            #get a reference counted buffer to pass downstream
            buff = self.pop_output_buffer(0)
            buff.offset = 0
            buff.length = len(payload)
            buff.get()[:] = numpy.fromstring(payload, numpy.uint8)

            self.post_output_tag(0, gras.Tag(0, "datagram", buff))
        else:
            print 'f',
            self.post_output_tag(0, gras.Tag(0, "fail", None))
