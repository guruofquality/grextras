#
# Copyright 2007-2012 Free Software Foundation, Inc.
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

import unittest
import gras
import grextras
import numpy
from demo_blocks import *

class test_delay(unittest.TestCase):

    def test_delay_simple(self):
        for delay in (0, 100, 1000, 10000):
            src_data = [1, 2, 3, 4, 5, 6, 7, 8]
            expected_result = tuple([0]*delay + src_data)

            src = VectorSource(numpy.float32, src_data)
            op = grextras.Delay(numpy.dtype(numpy.float32).itemsize)
            op.set_delay(delay)
            dst = VectorSink(numpy.float32)

            tb = gras.TopBlock()
            tb.connect(src, op, dst)
            tb.run()
            tb = None

            dst_data = dst.get_vector()
            self.assertEqual(expected_result, dst_data)

    #TODO test tag propagation
    #TODO test live change

if __name__ == '__main__':
    unittest.main()
