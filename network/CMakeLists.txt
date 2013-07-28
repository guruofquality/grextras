########################################################################
# Setup Boost
########################################################################
find_package(Boost COMPONENTS thread)
include_directories(${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})
list(APPEND GRAS_TOOL_LIBRARIES ${Boost_LIBRARIES})

########################################################################
## GrExtras Math Blocks
########################################################################
GRAS_TOOL(
    SOURCES
        deserializer.cpp deserializer.xml
        serializer.cpp serializer.xml
        socket_message.cpp  socket_message.xml
        tuntap.cpp tuntap.xml
    TARGET extras_network
    DIRECTORY extras
    COMPONENT extras
)

