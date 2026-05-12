##########################################################
### BASE
##########################################################

cmake_minimum_required(VERSION 3.24)
project(valentina_extensions C CXX)


##########################################################
### OPTIONS
##########################################################

set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build dependencies as static libraries" FORCE)


##########################################################
### EXTENSIONS
##########################################################

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")
include(ValentinaExtension)

add_subdirectory(compress)
add_subdirectory(base64)
add_subdirectory(generate_series)
add_subdirectory(ocr)
