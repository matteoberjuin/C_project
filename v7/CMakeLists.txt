cmake_minimum_required(VERSION 3.19)
project(Projet_CPP LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets)

qt_standard_project_setup()


qt_add_executable(Projet_CPP
    main.cpp
    Context.cpp
    Context.h
    Particle.h
    DrawArea.cpp
    DrawArea.h
    Vec2.h
)

target_link_libraries(Projet_CPP PRIVATE Qt6::Core Qt6::Widgets)

target_include_directories(Projet_CPP PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
