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
from gnuradio import gr

class test_stream_selector(unittest.TestCase):

    def test_stream_sel_simple(self):
        ss = grextras.StreamSelector()
        #ss.set_input_signature([4])
        #ss.set_input_signature(gras.IOSignature(4))
        #ss.set_output_signature(gras.IOSignature(4))

    #TODO test tag propagation
    #TODO test live change

if __name__ == '__main__':
    unittest.main()
