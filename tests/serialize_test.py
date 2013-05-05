# Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

import unittest
import gras
import grextras
import numpy
import time
from PMC import *

class RandomStuffSource(gras.Block):
    def __init__(self, tasks):
        gras.Block.__init__(self,
            name = "RandomStuffSource",
            out_sig = [numpy.uint32],
        )
        self._tasks = tasks

    def work(self, ins, outs):
        port = 0
        task, data = self._tasks[0]

        if task == "buff":
            n = min(len(outs[port]), len(data))
            outs[port][:n] = data[:n]
            self.produce(port, n)

        if task == "tag":
            offset = self.get_produced(port)
            tag = gras.Tag(offset, PMC_M(data))
            self.post_output_tag(port, tag)

        if task == "msg":
            msg = PMC_M(data)
            self.post_output_msg(port, msg)

        self._tasks = self._tasks[1:]
        if not self._tasks: self.mark_done()

class PktMsgSinkPrinter(gras.Block):
    def __init__(self):
        gras.Block.__init__(self,
            name = "PktMsgSinkPrinter",
            in_sig = [numpy.uint8],
        )

    def work(self, ins, outs):
        assert (len(ins[0]) == 0)

        msg = self.pop_input_msg(0)
        pkt_msg = msg()
        #print 'pop msg', msg, type(pkt_msg)
        if not isinstance(pkt_msg, gras.PacketMsg): return

        b = pkt_msg.buff.get().view('>i4') #big endian words32
        print 'buffer - words32', len(b)
        for i in range(len(b)):
            print 'b[%u] = 0x%08x'%(i, b[i])
        print '\n\n'

class test_serializer_blocks(unittest.TestCase):

    def setUp(self):
        self.tb = gras.TopBlock()

    def tearDown(self):
        self.tb = None

    def test_simple_ser(self):
        tasks = [
            ("tag", "hello"),
            ("buff", numpy.array([0, 1, 2, 3], numpy.uint32)),
            ("msg", "bye!"),
        ]

        src0 = RandomStuffSource(tasks)
        src1 = RandomStuffSource(tasks)
        ser = grextras.SerializePort()
        dst = PktMsgSinkPrinter()

        self.tb.connect(src0, (ser, 0))
        #self.tb.connect(src1, (ser, 1))
        self.tb.connect(ser, dst)
        self.tb.run()

if __name__ == '__main__':
    unittest.main()
