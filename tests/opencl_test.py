# Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

import unittest
import gras
import grextras
import numpy

class test_opencl_block(unittest.TestCase):

    def setUp(self):
        pass
        #self.tb = gras.TopBlock()

    def tearDown(self):
        pass
        #self.tb = None

    def test_making_block(self):
        o = grextras.OpenClBlock("GPU")

if __name__ == '__main__':
    unittest.main()

