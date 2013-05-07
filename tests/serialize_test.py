# Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

import unittest
import gras
import grextras
import numpy
import time
from PMC import *

########################################################################
## source stuff from a list of msgs/tags/buffs
########################################################################
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

########################################################################
## sink the received msgs/tags/buffs and store them
########################################################################
class RandomStuffSink(gras.Block):
    def __init__(self):
        gras.Block.__init__(self,
            name = "RandomStuffSink",
            in_sig = [numpy.uint32],
        )
        self._results = {"buff":numpy.array([], numpy.uint32), "tag":[], "msg":[]}

    def work(self, ins, outs):
        if len(ins[0]):
            self._results["buff"] = numpy.append(self._results["buff"], ins[0])
            self.consume(0, len(ins[0]))

        msg = self.pop_input_msg(0)
        if msg:
            data = msg()
            self._results["msg"].append(data)

    def propagate_tags(self, index, tag_iter):
        for tag in tag_iter:
            data = tag.object()
            self._results["tag"].append(data)

    def get_results(self):
        return self._results

########################################################################
## print input messages for debug help
########################################################################
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

        return
        b = pkt_msg.buff.get().view('>i4') #big endian words32
        print 'buffer - words32', len(b)
        for i in range(len(b)):
            print 'b[%u] = 0x%08x'%(i, b[i])
        print '\n\n'

########################################################################
## Begin unit testing class
########################################################################
class test_serializer_blocks(unittest.TestCase):

    def check_equal_helper(self, dst, tasks):

        def get_task_list(tn, ts):
            for name, data in ts:
                if name == tn: yield data

        def check_equal(type_name):
            print 'check_equal', type_name
            expected = list(get_task_list(type_name, tasks))
            result = dst.get_results()[type_name]
            self.assertEqual(len(expected), len(result))
            for i in range(len(expected)):
                print 'iteration', i
                self.assertEqual(expected[i], result[i])

        check_equal("tag")
        check_equal("msg")

        def get_super_buff(ts):
            a = numpy.array([], numpy.uint32)
            for b in get_task_list("buff", ts):
                a = numpy.append(a, b)
            return a

        print 'accumulating buffers and checking equal...'
        expected_buff = get_super_buff(tasks)
        result_buff = dst.get_results()["buff"]
        self.assertTrue((expected_buff == result_buff).all())

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
        ser = grextras.SerializePort(0, False) #mtu default, async ports
        ser.input_config(0).item_size = 4
        dst = PktMsgSinkPrinter()

        self.tb.connect(src0, (ser, 0))
        self.tb.connect(src1, (ser, 1))
        self.tb.connect(ser, dst)
        self.tb.run()

    def test_simple_loopback(self):
        #the tasks are different, this tests the async option as well
        tasks0 = [
            ("tag", "hello"),
            ("buff", numpy.array(numpy.random.randint(-300, +300, 100), numpy.uint32)),
            ("tag", 2.0 + 3j),
            ("buff", numpy.array(numpy.random.randint(-300, +300, 23), numpy.uint32)),
            ("msg", 16.2),
            ("buff", numpy.array(numpy.random.randint(-300, +300, 10), numpy.uint32)),
            ("buff", numpy.array(numpy.random.randint(-300, +300, 66), numpy.uint32)),
            ("msg", "bye!"),
        ]
        tasks1 = [
            ("tag", "hello2"),
            ("buff", numpy.array(numpy.random.randint(-300, +300, 20), numpy.uint32)),
            ("msg", 34.7),
            ("buff", numpy.array(numpy.random.randint(-300, +300, 11), numpy.uint32)),
            ("buff", numpy.array(numpy.random.randint(-300, +300, 45), numpy.uint32)),
            ("tag", 9.0 + -1j),
            ("buff", numpy.array(numpy.random.randint(-300, +300, 32), numpy.uint32)),
            ("msg", "cya"),
        ]

        src0 = RandomStuffSource(tasks0)
        src1 = RandomStuffSource(tasks1)
        ser = grextras.SerializePort(0, False) #mtu default, async ports
        ser.input_config(0).item_size = 4
        deser = grextras.DeserializePort()
        deser.output_config(0).item_size = 4
        dst0 = RandomStuffSink()
        dst1 = RandomStuffSink()

        self.tb.connect(src0, (ser, 0))
        self.tb.connect(src1, (ser, 1))
        self.tb.connect(ser, deser)
        self.tb.connect((deser, 0), dst0)
        self.tb.connect((deser, 1), dst1)
        self.tb.run()

        self.check_equal_helper(dst0, tasks0)
        self.check_equal_helper(dst1, tasks1)

    def test_recovery_over_stream(self):
        tasks = [
            ("tag", "hello"),
            ("buff", numpy.array(numpy.random.randint(-300, +300, 100), numpy.uint32)),
            ("tag", 2.0 + 3j),
            ("buff", numpy.array(numpy.random.randint(-300, +300, 23), numpy.uint32)),
            ("msg", 16.2),
            ("buff", numpy.array(numpy.random.randint(-300, +300, 10), numpy.uint32)),
            ("buff", numpy.array(numpy.random.randint(-300, +300, 66), numpy.uint32)),
            ("msg", "bye!"),
        ]

        src = RandomStuffSource(tasks)
        ser = grextras.SerializePort()
        ser.input_config(0).item_size = 4

        #these two slice up the datagrams
        #can we recover from such harsh slicing?
        d2s = grextras.Datagram2Stream(numpy.dtype(numpy.int32).itemsize)
        s2d = grextras.Stream2Datagram(numpy.dtype(numpy.int32).itemsize, 40) #mtu 40 bytes

        deser = grextras.DeserializePort(True) #true for recovery on
        deser.output_config(0).item_size = 4
        dst = RandomStuffSink()

        self.tb.connect(src, ser, d2s, s2d, deser, dst)
        self.tb.run()

        self.check_equal_helper(dst, tasks)

if __name__ == '__main__':
    unittest.main()
