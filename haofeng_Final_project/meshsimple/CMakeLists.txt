cmake_minimum_required(VERSION 3.15)
project(meshsimple)

set(CMAKE_CXX_STANDARD 17)

add_executable(meshsimple main.cpp Vec.h Global.h Object_Loader.h meshsimpleQEM.h tetrahedron.h)