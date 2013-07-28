# Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

import unittest
import gras
import TestUtils
import numpy
import time

class test_socket_message(unittest.TestCase):

    def setUp(self):
        self.tb = gras.TopBlock()

    def tearDown(self):
        self.tb = None

    def do_loop(self, to_socket, from_socket):
        src_data = tuple(numpy.random.randint(-1024, 1024, 10))
        src = TestUtils.VectorSource(numpy.int32, src_data)
        dst = TestUtils.VectorSink(numpy.int32)

        s2d = gras.Factory.make('/extras/stream_to_datagram', numpy.dtype(numpy.int32).itemsize, 0)
        d2s = gras.Factory.make('/extras/datagram_to_stream', numpy.dtype(numpy.int32).itemsize)

        self.tb.connect(src, s2d, to_socket)
        self.tb.connect(from_socket, d2s, dst)

        self.tb.start()
        time.sleep(0.5)
        self.tb.stop()
        self.tb.wait()
        self.tb.disconnect_all()

        self.assertEqual(src_data, dst.data())

    def test_udp_loopback(self):
        udp_server = gras.Factory.make('/extras/socket_message', "UDP_SERVER", 'localhost', '34567', 0)
        udp_client = gras.Factory.make('/extras/socket_message', "UDP_CLIENT", 'localhost', '34567', 0)

        #client sends to server - must be first
        self.do_loop(udp_client, udp_server)

        #server can reply to client
        self.do_loop(udp_server, udp_client)

    def test_tcp_loopback(self):
        tcp_server = gras.Factory.make('/extras/socket_message', "TCP_SERVER", 'localhost', '34567', 0)
        tcp_client = gras.Factory.make('/extras/socket_message', "TCP_CLIENT", 'localhost', '34567', 0)

        #client sends to server
        self.do_loop(tcp_client, tcp_server)

        #server reply to client
        self.do_loop(tcp_server, tcp_client)

        #a second client to replace the first connection
        print 'doing second client test now...'
        tcp_client2 = gras.Factory.make('/extras/socket_message', "TCP_CLIENT", 'localhost', '34567', 0)
        self.do_loop(tcp_client2, tcp_server) #must be first to establish new connection
        self.do_loop(tcp_server, tcp_client2)
        print '...done second client test'

if __name__ == '__main__':
    unittest.main()
