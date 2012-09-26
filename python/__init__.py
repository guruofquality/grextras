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

from extras_swig import *
import extras_pmt #act of importing performs injection
import block_gateway #act of importing performs injection
import pmt_to_python #act of importing performs injection

try: #it may not exist based on prereqs
    from transition_detect import transition_detect
except ImportError: pass

try: #it may not exist based on prereqs
    from msg_to_stdout import msg_to_stdout
except ImportError: pass

try: #it may not exist based on prereqs
    from simple_mac import simple_mac
except ImportError: pass

try: #it may not exist based on prereqs
    from append_key import append_key
except ImportError: pass

try: #it may not exist based on prereqs
    from heart_beat import heart_beat
except ImportError: pass

try: #it may not exist based on prereqs
    from burst_gate import burst_gate
except ImportError: pass

try: #it may not exist based on prereqs
    from tdma_engine import tdma_engine
except ImportError: pass

try: #it may not exist based on prereqs
    from packet_framer import *
except ImportError: pass

try: #it may not exist based on prereqs
    from channel_access_controller import *
except ImportError: pass

try: #it may not exist based on prereqs
    from virtual_channel_formatter import *
except ImportError: pass

try: #it may not exist based on prereqs
    from virtual_channel_mux import *
except ImportError: pass

from pmt_rpc import pmt_rpc

#backwards compatible contructor for old style suffix types
"""
add_cc = add_fc32_fc32
add_ff = add_f32_f32
add_ii = add_s32_s32
add_ss = add_s16_s16

multiply_cc = multiply_fc32_fc32
multiply_ff = multiply_f32_f32
multiply_ii = multiply_s32_s32
multiply_ss = multiply_s16_s16

sub_cc = subtract_fc32_fc32
sub_ff = subtract_f32_f32
sub_ii = subtract_s32_s32
sub_ss = subtract_s16_s16

divide_cc = divide_fc32_fc32
divide_ff = divide_f32_f32
divide_ii = divide_s32_s32
divide_ss = divide_s16_s16

#alias old gr_add_vXX and gr_multiply_vXX
add_vcc = add_cc
add_vff = add_ff
add_vii = add_ii
add_vss = add_ss
multiply_vcc = multiply_cc
multiply_vff = multiply_ff
multiply_vii = multiply_ii
multiply_vss = multiply_ss

#alias for old style const add/mults
def _fix_output(blk, op):
    if op == complex: cast = complex
    if op == float: cast = lambda x: x.real
    if op == int: cast = lambda x: int(x.real)
    out = blk.get_const()
    try: return map(cast, out)
    except: return cast(out)

def _inject_k(fcn, op, k):
    blk = fcn(k)
    setattr(blk, 'k', lambda: _fix_output(blk, op))
    setattr(blk, 'set_k', getattr(blk, 'set_const'))
    return blk

multiply_const_cc  = lambda k: _inject_k(multiply_const_fc32_fc32, complex, k)
multiply_const_vcc = lambda k: _inject_k(multiply_const_v_fc32_fc32, complex, k)
multiply_const_ff  = lambda k: _inject_k(multiply_const_f32_f32, float, k)
multiply_const_vff = lambda k: _inject_k(multiply_const_v_f32_f32, float, k)
multiply_const_ii  = lambda k: _inject_k(multiply_const_s32_s32, int, k)
multiply_const_vii = lambda k: _inject_k(multiply_const_v_s32_s32, int, k)
multiply_const_ss  = lambda k: _inject_k(multiply_const_s16_s16, int, k)
multiply_const_vss = lambda k: _inject_k(multiply_const_v_s16_s16, int, k)

add_const_cc  = lambda k: _inject_k(add_const_fc32_fc32, complex, k)
add_const_vcc = lambda k: _inject_k(add_const_v_fc32_fc32, complex, k)
add_const_ff  = lambda k: _inject_k(add_const_f32_f32, float, k)
add_const_vff = lambda k: _inject_k(add_const_v_f32_f32, float, k)
add_const_ii  = lambda k: _inject_k(add_const_s32_s32, int, k)
add_const_vii = lambda k: _inject_k(add_const_v_s32_s32, int, k)
add_const_ss  = lambda k: _inject_k(add_const_s16_s16, int, k)
add_const_vss = lambda k: _inject_k(add_const_v_s16_s16, int, k)
"""
