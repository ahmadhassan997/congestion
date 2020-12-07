
# Sets the minimum version of CMake required to build the native library.
cmake_minimum_required(VERSION 3.4.1)

include_directories(copa)
include_directories(pcc/src)
# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.
# rtcp
add_library(rtcp SHARED
        src/run_rtcp.cpp
        rtcp/rtcp.c
        rtcp/tcp.h)


add_library(copa SHARED
        src/run_copa.cpp
        copa/copa_receiver.cpp
        copa/udp_socket.cpp)

add_library(receiver SHARED
        pcc/app/appserver.cpp
        pcc/app/cc.h
        pcc/app/test_util.h
        pcc/src/udt.h
        pcc/src/ccc.cpp
        pcc/src/packet.cpp
        pcc/src/api.cpp
        pcc/src/common.cpp
        pcc/src/core.cpp
        pcc/src/buffer.cpp
        pcc/src/cache.cpp
        pcc/src/channel.cpp
        pcc/src/epoll.cpp
        pcc/src/list.cpp
        pcc/src/md5.cpp
        pcc/src/queue.cpp
        pcc/src/window.cpp)

add_library(pcc SHARED src/run_pcc.cpp)
add_library(vivace SHARED src/run_vivace.cpp)

# Searches for a specified prebuilt library and stores the path as a
# variable. Because CMake includes system libraries in the search path by
# default, you only need to specify the name of the public NDK library
# you want to add. CMake verifies that the library exists before
# completing its build.

find_library( # Sets the name of the path variable.
              log-lib

              # Specifies the name of the NDK library that
              # you want CMake to locate.
              log )

# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in this
# build script, prebuilt third-party libraries, or system libraries.

#target_link_libraries( # Specifies the target library.
#                       receiver
#
#                       # Links the target library to the log library
#                       # included in the NDK.
#                       ${log-lib} )

target_link_libraries(copa ${log-lib})
target_link_libraries(pcc receiver ${log-lib})
target_link_libraries(vivace receiver ${log-lib})
target_link_libraries(rtcp log)
