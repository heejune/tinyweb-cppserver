

cmake_minimum_required(VERSION 3.1)
project(tinyweb-cppserver)

set(CMAKE_CXX_STANDARD 14)
set(CXX_STANDARD  14)

set(CMAKE_EXPORT_COMPILE_COMMANDS 1 )
# -DCMAKE_EXPORT_COMPILE_COMMANDS=1

add_executable(tinyweb-cppserver tinyweb-cppserver.cpp)

set(Boost_USE_STATIC_LIBS        OFF)
set(Boost_USE_MULTITHREADED      ON)
set(Boost_USE_STATIC_RUNTIME     OFF)
set(BOOST_ALL_DYN_LINK           ON)   # force dynamic linking for all libraries

find_package(Boost 1.64 REQUIRED COMPONENTS
  thread system filesystem)

add_definitions(-DBOOST_ALL_NO_LIB)  # tell the compiler to undefine this boost macro

target_compile_features(tinyweb-cppserver PRIVATE cxx_constexpr cxx_variadic_templates )

SET (CMAKE_AR      "/usr/bin/llvm-ar")
SET (CMAKE_LINKER  "/usr/bin/llvm-ld")
SET (CMAKE_NM      "/usr/bin/llvm-nm")
SET (CMAKE_OBJDUMP "/usr/bin/llvm-objdump")
SET (CMAKE_RANLIB  "/usr/bin/llvm-ranlib")

if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS}) 
    target_link_libraries(tinyweb-cppserver ${Boost_LIBRARIES})
    link_directories(${Boost_LIBRARY_DIR})
endif()

# cmake -DBOOST_ROOT=/home/hekim/devtools/boost_1_64_0/ ..
