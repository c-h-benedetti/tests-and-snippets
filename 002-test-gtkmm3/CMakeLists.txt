cmake_minimum_required(VERSION 3.10)
project(MyGtkmmApp)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTKMM REQUIRED gtkmm-3.0)
pkg_check_modules(EPOXY REQUIRED epoxy)

include_directories(${EPOXY_INCLUDE_DIRS} ${GTKMM_INCLUDE_DIRS})
link_directories(${EPOXY_LIBRARY_DIRS} ${GTKMM_LIBRARY_DIRS})

add_executable(MyGtkmmApp 
    minimal_gtk_epoxy.hpp
    minimal_gtk_epoxy.cpp
    ogl_window.hpp
    ogl_window.cpp
    several-windows.hpp
    several-windows.cpp
    main.cpp
)

target_link_libraries(MyGtkmmApp ${EPOXY_LIBRARIES} ${GTKMM_LIBRARIES})
