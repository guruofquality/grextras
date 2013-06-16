########################################################################
# Install GrExtras include files
########################################################################
install(FILES

    config.hpp

    add_const.hpp
    add.hpp
    divide.hpp
    multiply_const.hpp
    multiply.hpp
    subtract.hpp

    delay.hpp
    burst_tagger.hpp
    stream_selector.hpp

    noise_source.hpp
    signal_source.hpp

    datagram_to_stream.hpp
    stream_to_datagram.hpp
    socket_message.hpp
    tuntap.hpp

    opencl_block.hpp
    orc_block.hpp
    clang_block.hpp

    uhd_control_port.hpp
    uhd_status_port.hpp

    serialize_port.hpp
    deserialize_port.hpp

    DESTINATION include/grextras
    COMPONENT "extras_devel"
)
