cmake_minimum_required(VERSION 3.10)
project(PythonEmbed C CXX)

# Configurer les options de compilation
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Ajouter CPython en tant que sous-projet
set(Python_ADDITIONAL_VERSIONS 3.8)
include(ExternalProject)
ExternalProject_Add(cpython
    GIT_REPOSITORY https://github.com/python/cpython.git
    GIT_TAG v3.8.12
    CONFIGURE_COMMAND ./configure --enable-shared=no --enable-optimizations
    BUILD_COMMAND make -j2 -s
    BUILD_IN_SOURCE 1
    INSTALL_COMMAND ""
    STEP_TARGETS build
)

# Liste des fichiers sources
set(SOURCES main.cpp)

# Créer l'exécutable
add_executable(PythonEmbed ${SOURCES})

# Lier les bibliothèques
add_dependencies(PythonEmbed cpython-build)
ExternalProject_Get_Property(cpython BINARY_DIR)
target_link_libraries(PythonEmbed ${BINARY_DIR}/libpython3.8m.a)
target_link_libraries(PythonEmbed pthread dl util m)

# Chemins d'inclusion pour la bibliothèque Python
ExternalProject_Get_Property(cpython SOURCE_DIR)
include_directories(${SOURCE_DIR}/Include)
include_directories(${BINARY_DIR})