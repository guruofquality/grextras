# Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

import unittest
import gras
import grextras
import numpy

SOURCE = """
.function volk_32f_x2_add_32f_a_orc_impl
.dest 4 dst
.source 4 src1
.source 4 src2
addf dst, src1, src2

.function volk_32f_sqrt_32f_a_orc_impl
.source 4 src
.dest 4 dst
sqrtf dst, src
"""

class test_orc_block(unittest.TestCase):

    def setUp(self):
        self.tb = gras.TopBlock()

    def tearDown(self):
        self.tb = None

    def test_compile(self):
        op = grextras.OrcBlock()
        op.set_program('volk_32f_x2_add_32f_a_orc_impl', SOURCE)

if __name__ == '__main__':
    unittest.main()

