# CMakeList.txt : CMake project for OpSrv, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

# Add source to this project's executable.
add_executable (OpSrv
    "WSAInitializer.cpp" "WSAInitializer.h"
    "SocketListen.cpp" "SocketListen.h"
    "Protocol.cpp" "Protocol.h"
    "MessageDecoder.cpp" "MessageDecoder.h"
    "OpSrv.cpp")

add_definitions(/std:c++17)

if(WIN32)
  target_link_libraries(OpSrv wsock32 ws2_32)
endif()

# TODO: Add tests and install targets if needed.
