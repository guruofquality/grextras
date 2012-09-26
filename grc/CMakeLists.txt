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

########################################################################
# Install GRC xml files
########################################################################
if(UHD_FOUND)
    list(APPEND grc_sources
        extras_uhd_amsg_source.xml
    )
endif()

list(APPEND grc_sources
    extras_add_const.xml
    extras_block_tree.xml
    extras_divide.xml
    extras_multiply.xml
    extras_signal_source.xml
    extras_subtract.xml
    extras_add.xml
    extras_delay.xml
    extras_multiply_const.xml
    extras_noise_source.xml
    extras_decim_fir.xml
    extras_blob_to_socket.xml
    extras_blob_to_stream.xml
    extras_socket_to_blob.xml
    extras_stream_to_blob.xml
    extras_tuntap.xml
    extras_packet_framer.xml
    extras_packet_deframer.xml
    extras_stream_selector.xml
    extras_pmt_rpc.xml
    extras_msg_many_to_one.xml
    extras_socket_msg.xml
    extras_transition_detect.xml
    extras_msg_to_stdout.xml
    simple_mac.xml
    extras_append_key.xml
    heart_beat.xml
    burst_gate.xml
    channel_access_controller.xml
    tdma_engine.xml
    virtual_channel_formatter.xml
    virtual_channel_mux.xml
    virtual_channel_demux.xml
)

install(
    FILES ${grc_sources}
    DESTINATION ${GRC_BLOCKS_DIR}
    COMPONENT "extras_python"
)
