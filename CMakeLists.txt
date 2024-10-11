cmake_minimum_required(VERSION 3.27)
project(tt)

set(CMAKE_CXX_STANDARD 14)

include_directories(easy x)
include_directories(easy x/include)
link_directories(easy x/lib64)
link_libraries(libeasyx.a)

add_executable(tt
        ex/include/easyx.h
        ex/include/graphics.h
        ex/lib64/libeasyx.a
        main.cpp)
