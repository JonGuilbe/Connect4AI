cmake_minimum_required(VERSION 3.3)
project(FPFrame)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES
    arupsoldplayer.c
    arupsoldplayer.h
    con4lib.c
    con4lib.h
    connect4.c
    firstavailable.c
    firstavailable.h group14.c group14.h)

add_executable(FPFrame ${SOURCE_FILES})