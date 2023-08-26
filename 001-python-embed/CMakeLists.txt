cmake_minimum_required(VERSION 3.16 FATAL_ERROR)
project(PythonEmbed LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Add CPython as a sub-project.
set(Python_ADDITIONAL_VERSIONS 3.10.12)

include(ExternalProject)
set(CPYTHON_BUILD_DIR ${CMAKE_BINARY_DIR}/cpython_build)
set(CPYTHON_STAMP_DIR ${CPYTHON_BUILD_DIR}/stamp) 
set(CPYTHON_INCLUDE_DIR ${CPYTHON_BUILD_DIR}/src/cpython/Include)
set(CPYTHON_LIBRARIES_DIR ${CPYTHON_BUILD_DIR}/src/cpython/Lib)
set(CPYTHON_INTERPRETER ${CPYTHON_BUILD_DIR}/src/cpython/python)
set(CPYTHON_SOURCES_DIR ${CPYTHON_BUILD_DIR}/src/cpython)

ExternalProject_Add(cpython
    PREFIX ${CPYTHON_BUILD_DIR}
    STAMP_DIR ${CPYTHON_STAMP_DIR}
    GIT_REPOSITORY https://github.com/python/cpython.git
    GIT_TAG v3.10.12
    CONFIGURE_COMMAND bash -c "[ ! -f Makefile ] && ./configure --enable-shared --enable-optimizations || true"
    BUILD_COMMAND make -j6
    BUILD_IN_SOURCE 1
    INSTALL_COMMAND ""
    STEP_TARGETS build
)

# List sources
set(SOURCES
    Custom.h
    Custom.c
    Point2D.h
    Point2D.c
    spammodule.hpp
    spammodule.cpp
    initTest.cpp
)

# Create the executable
add_executable(PythonEmbed ${SOURCES})

target_compile_definitions(PythonEmbed PRIVATE CPYTHON_BUILD_DIR=\"${CPYTHON_SOURCES_DIR}\")

target_include_directories(PythonEmbed
	PUBLIC ${CPYTHON_INCLUDE_DIR}
    PUBLIC ${CPYTHON_SOURCES_DIR}
    PUBLIC ${BINARY_DIR}
)

# Link libraries
add_dependencies(PythonEmbed cpython)

target_link_libraries(PythonEmbed 
    ${CPYTHON_BUILD_DIR}/src/cpython/libpython3.10.so.1.0
    pthread 
    dl 
    util 
    m
)
