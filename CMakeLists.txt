

cmake_minimum_required(VERSION 3.1)
project(tinyweb-cppserver)
add_executable(tinyweb-cppserver tinyweb-cppserver.cpp)

set(Boost_USE_STATIC_LIBS        OFF)
set(Boost_USE_MULTITHREADED      ON)
set(Boost_USE_STATIC_RUNTIME     OFF)
set(BOOST_ALL_DYN_LINK           ON)   # force dynamic linking for all libraries

find_package(Boost 1.64 REQUIRED COMPONENTS
  system filesystem)

add_definitions(-DBOOST_ALL_NO_LIB)  # tell the compiler to undefine this boost macro

if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS}) 
    target_link_libraries(tinyweb-cppserver ${Boost_LIBRARIES})
    link_directories(${Boost_LIBRARY_DIR})
endif()
