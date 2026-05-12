##########################################################
### DEPENDENCIES
##########################################################

include(FetchContent)
include(ExternalProject)
include(StaticZlib)

function(write_find_package_redirect package content)
    string(TOLOWER "${package}" package_lower)
    file(WRITE "${CMAKE_FIND_PACKAGE_REDIRECTS_DIR}/${package}Config.cmake" "${content}")
    file(WRITE "${CMAKE_FIND_PACKAGE_REDIRECTS_DIR}/${package_lower}-config.cmake" "${content}")
endfunction()

# JPEG
set(jpeg_turbo_INSTALL_DIR ${CMAKE_BINARY_DIR}/_deps/jpeg_turbo-install)
file(MAKE_DIRECTORY ${jpeg_turbo_INSTALL_DIR}/include ${jpeg_turbo_INSTALL_DIR}/lib)
if(MSVC)
    set(jpeg_turbo_LIBRARY ${jpeg_turbo_INSTALL_DIR}/lib/jpeg-static.lib)
else()
    set(jpeg_turbo_LIBRARY ${jpeg_turbo_INSTALL_DIR}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}jpeg${CMAKE_STATIC_LIBRARY_SUFFIX})
endif()

if(CMAKE_CONFIGURATION_TYPES)
    set(jpeg_turbo_BUILD_TYPE_ARGS)
    set(jpeg_turbo_STEP_ARGS
        BUILD_COMMAND ${CMAKE_COMMAND} --build <BINARY_DIR> --config $<CONFIG>
        INSTALL_COMMAND ${CMAKE_COMMAND} --install <BINARY_DIR> --config $<CONFIG>)
else()
    set(jpeg_turbo_BUILD_TYPE_ARGS -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE})
    set(jpeg_turbo_STEP_ARGS)
endif()

ExternalProject_Add(
    jpeg_turbo
    PREFIX         ${CMAKE_BINARY_DIR}/_deps/jpeg_turbo
    GIT_REPOSITORY https://github.com/libjpeg-turbo/libjpeg-turbo.git
    GIT_TAG        3.1.4.1
    GIT_SHALLOW    TRUE
    CMAKE_ARGS
        ${jpeg_turbo_BUILD_TYPE_ARGS}
        -DCMAKE_INSTALL_PREFIX=${jpeg_turbo_INSTALL_DIR}
        -DCMAKE_INSTALL_LIBDIR=lib
        -DCMAKE_POSITION_INDEPENDENT_CODE=ON
        -DENABLE_SHARED=OFF
        -DENABLE_STATIC=ON
        -DWITH_JAVA=OFF
        -DWITH_TESTS=OFF
        -DWITH_TOOLS=OFF
        -DWITH_TURBOJPEG=OFF
    ${jpeg_turbo_STEP_ARGS}
    BUILD_BYPRODUCTS ${jpeg_turbo_LIBRARY})

add_library(JPEG::JPEG STATIC IMPORTED GLOBAL)
set_target_properties(JPEG::JPEG PROPERTIES
    IMPORTED_LOCATION ${jpeg_turbo_LIBRARY}
    INTERFACE_INCLUDE_DIRECTORIES ${jpeg_turbo_INSTALL_DIR}/include)
add_dependencies(JPEG::JPEG jpeg_turbo)

write_find_package_redirect(JPEG
"if(NOT TARGET JPEG::JPEG)
    add_library(JPEG::JPEG INTERFACE IMPORTED)
    set_target_properties(JPEG::JPEG PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES \"${jpeg_turbo_INSTALL_DIR}/include\"
        INTERFACE_LINK_LIBRARIES \"${jpeg_turbo_LIBRARY}\")
endif()
set(JPEG_FOUND TRUE)
set(JPEG_VERSION 80)
set(JPEG_INCLUDE_DIR \"${jpeg_turbo_INSTALL_DIR}/include\")
set(JPEG_INCLUDE_DIRS \"${jpeg_turbo_INSTALL_DIR}/include\")
set(JPEG_LIBRARY JPEG::JPEG)
set(JPEG_LIBRARIES JPEG::JPEG)
")

# PNG
set(PNG_SHARED OFF CACHE BOOL "Do not build libpng shared library" FORCE)
set(PNG_STATIC ON CACHE BOOL "Build libpng static library" FORCE)
set(PNG_TESTS OFF CACHE BOOL "Do not build libpng tests" FORCE)
set(PNG_TOOLS OFF CACHE BOOL "Do not build libpng tools" FORCE)

FetchContent_Declare(
    libpng
    GIT_REPOSITORY https://github.com/pnggroup/libpng.git
    GIT_TAG        v1.6.58
    GIT_SHALLOW    TRUE)

FetchContent_MakeAvailable(libpng)

write_find_package_redirect(PNG
"if(NOT TARGET PNG::PNG)
    add_library(PNG::PNG INTERFACE IMPORTED)
    set_target_properties(PNG::PNG PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES \"${libpng_SOURCE_DIR};${libpng_BINARY_DIR}\"
        INTERFACE_LINK_LIBRARIES png_static)
endif()
set(PNG_FOUND TRUE)
set(PNG_VERSION 1.6.58)
set(PNG_INCLUDE_DIR \"${libpng_SOURCE_DIR};${libpng_BINARY_DIR}\")
set(PNG_INCLUDE_DIRS \"${libpng_SOURCE_DIR};${libpng_BINARY_DIR}\")
set(PNG_LIBRARY PNG::PNG)
set(PNG_LIBRARIES PNG::PNG)
")

# LEPTONICA
set(BUILD_PROG OFF CACHE BOOL "Do not build Leptonica utility programs" FORCE)
set(SW_BUILD OFF CACHE BOOL "Do not use Software Network dependencies" FORCE)
set(SYM_LINK OFF CACHE BOOL "Do not create Leptonica symlink target" FORCE)

FetchContent_Declare(
    leptonica
    GIT_REPOSITORY https://github.com/DanBloomberg/leptonica.git
    GIT_TAG        1.86.0
    GIT_SHALLOW    TRUE)

FetchContent_MakeAvailable(leptonica)
add_dependencies(leptonica jpeg_turbo)

target_include_directories(leptonica
    PUBLIC
        $<BUILD_INTERFACE:${leptonica_SOURCE_DIR}/src>
        $<BUILD_INTERFACE:${leptonica_BINARY_DIR}/src>)

file(WRITE ${leptonica_BINARY_DIR}/LeptonicaTargets.cmake "")
file(APPEND ${leptonica_BINARY_DIR}/LeptonicaConfig.cmake
    "\nset(Leptonica_INCLUDE_DIRS \"${leptonica_SOURCE_DIR}/src;${leptonica_BINARY_DIR}/src\")\n"
    "set(Leptonica_LIBRARIES leptonica)\n")

set(Leptonica_DIR ${leptonica_BINARY_DIR} CACHE PATH "Leptonica package directory" FORCE)

set(BUILD_TRAINING_TOOLS OFF CACHE BOOL "Do not build Tesseract training tools" FORCE)
set(BUILD_TESTS OFF CACHE BOOL "Do not build Tesseract tests" FORCE)
set(DISABLE_ARCHIVE ON CACHE BOOL "Disable libarchive support in Tesseract" FORCE)
set(DISABLE_CURL ON CACHE BOOL "Disable libcurl support in Tesseract" FORCE)
set(DISABLE_TIFF ON CACHE BOOL "Disable libtiff support in Tesseract" FORCE)
set(GRAPHICS_DISABLED ON CACHE BOOL "Disable Tesseract ScrollView graphics" FORCE)
set(INSTALL_CONFIGS OFF CACHE BOOL "Do not install Tesseract configs" FORCE)
set(OPENMP_BUILD OFF CACHE BOOL "Do not use OpenMP in Tesseract" FORCE)

# TESSERACT
FetchContent_Declare(
    tesseract
    GIT_REPOSITORY https://github.com/tesseract-ocr/tesseract.git
    GIT_TAG        5.5.2
    GIT_SHALLOW    TRUE
    GIT_SUBMODULES "")

FetchContent_MakeAvailable(tesseract)


##########################################################
### TARGET
##########################################################

add_valentina_extension(
    NAME ocr
    SOURCES ocr.cpp
    INCLUDE_DIRECTORIES
        ${tesseract_BINARY_DIR}/include
        ${leptonica_SOURCE_DIR}/src
        ${leptonica_BINARY_DIR}/src
    LINK_LIBRARIES
        libtesseract
        ${CMAKE_DL_LIBS}
    DATA_DIRS
        ${CMAKE_CURRENT_LIST_DIR}/tessdata)
