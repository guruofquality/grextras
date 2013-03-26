########################################################################
# Install GRC xml files
########################################################################

list(APPEND grc_sources
    extras_block_tree.xml

    extras_add.xml
    extras_add_const.xml
    extras_subtract.xml
    extras_multiply.xml
    extras_multiply_const.xml
    extras_divide.xml

    extras_noise_source.xml
    extras_signal_source.xml

    extras_datagram_to_stream.xml
    extras_stream_to_datagram.xml

    extras_delay.xml
    extras_tuntap.xml
    extras_stream_selector.xml
    extras_socket_message.xml

    extras_packet_framer.xml
    extras_packet_deframer.xml

    extras_query_server.xml
)

install(
    FILES ${grc_sources}
    DESTINATION share/gnuradio/grc/blocks
    COMPONENT "extras_python"
)
