# Copyright 2011-2012 Free Software Foundation, Inc.
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

import numpy
import extras_swig

#hacky way so we can import in-tree
try: import pmt
except ImportError: from gruel import pmt

for name in dir(extras_swig):
    if 'pmt' in name:
        setattr(pmt, name, getattr(extras_swig, name))
        setattr(pmt, name.replace('ext_blob', 'blob'), getattr(extras_swig, name))

#this function knows how to convert an address to a numpy array
def __pointer_to_ndarray(addr, nitems):
    dtype = numpy.dtype(numpy.uint8)
    class array_like:
        __array_interface__ = {
            'data' : (int(addr), False),
            'typestr' : dtype.base.str,
            'descr' : dtype.base.descr,
            'shape' : (nitems,) + dtype.shape,
            'strides' : None,
            'version' : 3
        }
    return numpy.asarray(array_like()).view(dtype.base)

#re-create the blob data functions, but yield a numpy array instead
def pmt_blob_data(blob):
    return __pointer_to_ndarray(extras_swig.pmt_blob_rw_data(blob), extras_swig.pmt_blob_length(blob))

#re-create mgr acquire by calling into python GIL-safe version
class pmt_mgr:
    def __init__(self): self._mgr = extras_swig.pmt_mgr()
    def set(self, x): self._mgr.set(x)
    def reset(self, x): self._mgr.reset(x)
    def acquire(self, block = True): return extras_swig.pmt_mgr_acquire_safe(self._mgr, block)

#inject it into the pmt namespace
pmt.pmt_make_blob = extras_swig.pmt_make_blob
pmt.pmt_blob_data = pmt_blob_data #both call rw data, numpy isnt good with const void *
pmt.pmt_blob_rw_data = pmt_blob_data
pmt.pmt_blob_resize = extras_swig.pmt_blob_resize
pmt.pmt_mgr = pmt_mgr
