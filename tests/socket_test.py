# Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

import unittest
import gras
import grextras
import numpy
import time

class test_socket_message(unittest.TestCase):

    def setUp(self):
        self.tb = gras.TopBlock()

    def tearDown(self):
        self.tb = None

    def test_udp_loopback(self):
        udp_client = grextras.SocketMessage("UDP_CLIENT", 'localhost', '34567')
        udp_server = grextras.SocketMessage("UDP_SERVER", 'localhost', '34567')

    

if __name__ == '__main__':
    unittest.main()
