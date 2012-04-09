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

import extras_pmt #injects into pmt python namespace, internal unit test only!
from gnuradio import gr, gr_unittest

#hacky way so we can import in-tree
try: import pmt
except: from gruel import pmt

import numpy

class test_delay(gr_unittest.TestCase):

    def setUp (self):
        self.tb = gr.top_block ()

    def tearDown (self):
        self.tb = None

    def test03 (self):
        randints = numpy.random.randint(0, 256, 123)
        blob = pmt.pmt_make_blob(len(randints))
        blob_data = pmt.pmt_blob_data(blob)

        blob_data[:] = randints #assign rand ints to data
        self.assertItemsEqual(blob_data, randints)

        blob_data = pmt.pmt_blob_data(blob)
        self.assertItemsEqual(blob_data, randints)

    def test04 (self):
        try: mgr = pmt.pmt_mgr()
        except: return #may not be implemented
        mgr.set(pmt.pmt_make_blob(100))
        mgr.set(pmt.pmt_make_blob(100))
        a = mgr.acquire(False)
        b = mgr.acquire(False)
        c = mgr.acquire(False)
        self.assertTrue(not pmt.pmt_is_null(a))
        self.assertTrue(not pmt.pmt_is_null(b))
        self.assertTrue(pmt.pmt_is_null(c))

if __name__ == '__main__':
    gr_unittest.run(test_delay, "test_delay.xml")
