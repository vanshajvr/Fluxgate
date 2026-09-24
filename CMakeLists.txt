cmake_minimum_required(VERSION 3.16)
project(fluxgate CXX)

if(POLICY CMP0167)
  cmake_policy(SET CMP0167 NEW)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)   # <-- new: needed for IntelliSense fix below

find_package(Boost REQUIRED COMPONENTS system)
find_package(Threads REQUIRED)

add_executable(fluxgate
    src/main.cpp
    src/server.cpp
    src/session.cpp
)
target_link_libraries(fluxgate PRIVATE Boost::headers Boost::system Threads::Threads)

target_compile_options(fluxgate PRIVATE -Wall -Wextra)