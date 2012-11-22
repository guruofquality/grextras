########################################################################
# Install GrExtras include files
########################################################################
install(FILES

    config.hpp

    add_const.hpp
    add.hpp
    delay.hpp
    divide.hpp
    multiply_const.hpp
    multiply.hpp
    subtract.hpp
    stream_selector.hpp
    noise_source.hpp
    signal_source.hpp
    datagram_to_stream.hpp
    stream_to_datagram.hpp
    socket_message.hpp
    tuntap.hpp

    DESTINATION include/grextras
    COMPONENT "extras_devel"
)
