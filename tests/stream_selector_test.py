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

class test_stream_selector(unittest.TestCase):

    def test_stream_selector_simple(self):
        ss = grextras.StreamSelector(4)
        ss.set_paths([1, 0]) #in[0] -> out[1], in[1] -> out[0]

        src0 = grextras.VectorSource(numpy.float32, [1, 2, 3, 4])
        src1 = grextras.VectorSource(numpy.float32, [5, 6, 7, 8])

        dst0 = grextras.VectorSink(numpy.float32)
        dst1 = grextras.VectorSink(numpy.float32)

        tb = gras.TopBlock()
        tb.connect(src0, (ss, 0), dst0)
        tb.connect(src1, (ss, 1), dst1)
        tb.run()
        tb = None

        #FIXME can be EMPTY! since done logic means any input port done.
        # This might finish with only one port of data forwarded.
        # This is OK, but it might be fixed by configurable done logic.

        if dst1.data(): self.assertEqual((1, 2, 3, 4), dst1.data())
        if dst0.data(): self.assertEqual((5, 6, 7, 8), dst0.data())

    #TODO test non forwarding inputs
    #TODO test tag propagation
    #TODO test live change

if __name__ == '__main__':
    unittest.main()
