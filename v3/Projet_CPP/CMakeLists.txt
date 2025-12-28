cmake_minimum_required(VERSION 3.19)
project(Projet_CPP LANGUAGES CXX)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core)
find_package(Qt6 REQUIRED COMPONENTS Widgets)

qt_standard_project_setup()

qt_add_executable(Projet_CPP
    main.cpp
    simulation/Vec2.h
    simulation/Particle.h
    simulation/Context.h
    simulation/Context.cpp
    simulation/drawarea.h simulation/drawarea.cpp
)

target_link_libraries(Projet_CPP
    PRIVATE
        Qt::Core
)
target_link_libraries(Projet_CPP PRIVATE Qt6::Widgets)
target_include_directories(Projet_CPP PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/simulation
)


include(GNUInstallDirs)

install(TARGETS Projet_CPP
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)
