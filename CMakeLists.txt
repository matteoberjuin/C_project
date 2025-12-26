cmake_minimum_required(VERSION 4.0)
project(projet)

set(CMAKE_CXX_STANDARD 20)

add_executable(projet main.cpp
        Particle.h
        Vec2.h
        Context.h
        Context.cpp)
