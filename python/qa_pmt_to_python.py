#!/usr/bin/env python
#
# Copyright 2012 Free Software Foundation, Inc.
# 
# This file is part of GNU Radio
# 
# GNU Radio is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3, or (at your option)
# any later version.
# 
# GNU Radio is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with GNU Radio; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street,
# Boston, MA 02110-1301, USA.
# 

from gnuradio import gr, gr_unittest
import extras_pmt #injects
import pmt_to_python #injects
import extras_swig as extras

#hacky way so we can import in-tree
try: import pmt
except ImportError: from gruel import pmt

import numpy

class test_pmt_to_python(gr_unittest.TestCase):

    def loopback(self, python_data):
        as_a_pmt = pmt.from_python(python_data)
        back_to_python = pmt.to_python(as_a_pmt)
        self.assertEqual(python_data, back_to_python)

    def test_null(self):
        self.assertEqual(None, pmt.to_python(pmt.PMT_NIL))
        self.assertTrue(pmt.pmt_equal(pmt.PMT_NIL, pmt.from_python(None)))

    def test_basic_types(self):
        self.assertEqual(42, pmt.to_python(pmt.pmt_from_long(42)))
        self.assertTrue(pmt.pmt_equal(pmt.pmt_from_long(42), pmt.from_python(42)))

        self.assertEqual(4.2, pmt.to_python(pmt.pmt_from_double(4.2)))
        self.assertTrue(pmt.pmt_equal(pmt.pmt_from_double(4.2), pmt.from_python(4.2)))

        self.loopback(None)
        self.loopback(42)
        self.loopback(4.2)
        self.loopback(4.2j)
        self.loopback("42")

    def test_iterables(self):
        self.loopback(tuple())
        self.loopback(list())
        #self.loopback(dict()) #FIXME This is a pmt bug, empty dicts are the same as NIL
        self.loopback((1, 2, 3, 4))
        self.loopback([1, 2, 3, 4])
        self.loopback({1:2, 3:4})

    def test_nested(self):
        self.loopback([(1, "two"), {3 : 4.0}, [5, (6j, )]])
        self.loopback({'channel': 123, 'timestamp': (100L, 0.2)})

    def test_time_tuple(self):
        timestamp = pmt.pmt_make_tuple(pmt.pmt_from_uint64(123), pmt.pmt_from_double(0.123))
        ts = pmt.to_python(timestamp)
        self.assertTrue(isinstance(ts[0], long));
        self.assertTrue(isinstance(ts[1], float));

    def test_numpy(self):
        python_data = numpy.array([1, 2, 3], numpy.uint8)
        as_a_pmt = pmt.from_python(python_data)
        back_to_python = pmt.to_python(as_a_pmt)
        self.assertTrue((python_data == back_to_python).all())

if __name__ == '__main__':
    gr_unittest.run(test_pmt_to_python, "test_pmt_to_python.xml")
