cmake_minimum_required(VERSION 3.10)
project(MyGtkmmApp)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTKMM REQUIRED gtkmm-3.0)

include_directories(${GTKMM_INCLUDE_DIRS})
link_directories(${GTKMM_LIBRARY_DIRS})
add_definitions(${GTKMM_CFLAGS_OTHER})

add_executable(MyGtkmmApp 
main.cpp
Experimental.hpp
Experimental.cpp
several-windows.hpp
several-windows.cpp)

target_link_libraries(MyGtkmmApp ${GTKMM_LIBRARIES})
