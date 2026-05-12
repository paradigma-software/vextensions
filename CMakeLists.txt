##########################################################
### BASE
##########################################################

cmake_minimum_required(VERSION 3.24)
project(ocr C CXX)


##########################################################
### OPTIONS
##########################################################

set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build dependencies as static libraries" FORCE)


##########################################################
### DATA
##########################################################

set(OCR_TESSDATA_DIR ${CMAKE_CURRENT_LIST_DIR}/tessdata)
set(OCR_TESSDATA_LANGS
    ukr
    eng
    pol
    deu
    fra
    ita
    spa
    por
    chi_sim
    chi_tra
    jpn
    kor)

foreach(lang IN LISTS OCR_TESSDATA_LANGS)
    set(traineddata ${OCR_TESSDATA_DIR}/${lang}.traineddata)

    if(NOT EXISTS ${traineddata})
        message(FATAL_ERROR "Missing ${traineddata}")
    endif()

    list(APPEND OCR_TESSDATA_FILES ${traineddata})
endforeach()


##########################################################
### DEPENDENCIES
##########################################################

include(FetchContent)
include(ExternalProject)

function(write_find_package_redirect package content)
    string(TOLOWER "${package}" package_lower)
    file(WRITE "${CMAKE_FIND_PACKAGE_REDIRECTS_DIR}/${package}Config.cmake" "${content}")
    file(WRITE "${CMAKE_FIND_PACKAGE_REDIRECTS_DIR}/${package_lower}-config.cmake" "${content}")
endfunction()

# ZLIB
set(ZLIB_BUILD_EXAMPLES OFF CACHE BOOL "Do not build zlib examples" FORCE)

FetchContent_Declare(
    zlib
    GIT_REPOSITORY https://github.com/madler/zlib.git
    GIT_TAG        v1.3.1
    GIT_SHALLOW    TRUE)

FetchContent_MakeAvailable(zlib)

write_find_package_redirect(ZLIB
"if(NOT TARGET ZLIB::ZLIB)
    add_library(ZLIB::ZLIB INTERFACE IMPORTED)
    set_target_properties(ZLIB::ZLIB PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES \"${zlib_SOURCE_DIR};${zlib_BINARY_DIR}\"
        INTERFACE_LINK_LIBRARIES zlibstatic)
endif()
set(ZLIB_FOUND TRUE)
set(ZLIB_VERSION 1.3.1)
set(ZLIB_INCLUDE_DIR \"${zlib_SOURCE_DIR};${zlib_BINARY_DIR}\")
set(ZLIB_INCLUDE_DIRS \"${zlib_SOURCE_DIR};${zlib_BINARY_DIR}\")
set(ZLIB_LIBRARY ZLIB::ZLIB)
set(ZLIB_LIBRARIES ZLIB::ZLIB)
")

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
### TARGETS
##########################################################

add_library(ocr MODULE ocr.cpp)
target_compile_features(ocr PRIVATE cxx_std_20)


##########################################################
### INCLUDE DIRECTORIES
##########################################################

target_include_directories(ocr
    PRIVATE
        ${tesseract_BINARY_DIR}/include
        ${leptonica_SOURCE_DIR}/src
        ${leptonica_BINARY_DIR}/src)


##########################################################
### LINKER
##########################################################

target_link_libraries(ocr PRIVATE libtesseract ${CMAKE_DL_LIBS})

foreach(traineddata IN LISTS OCR_TESSDATA_FILES)
    get_filename_component(traineddata_name ${traineddata} NAME)
    list(APPEND OCR_COPY_TESSDATA_COMMANDS
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${traineddata}
            "$<TARGET_FILE_DIR:ocr>/tessdata/${traineddata_name}")
    list(APPEND OCR_PACKAGE_TESSDATA_COMMANDS
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${traineddata}
            "${CMAKE_BINARY_DIR}/package/$<CONFIG>/ocr/tessdata/${traineddata_name}")
endforeach()

add_custom_command(
    TARGET ocr POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:ocr>/tessdata"
    ${OCR_COPY_TESSDATA_COMMANDS}
    COMMAND ${CMAKE_COMMAND} -E rm -rf "${CMAKE_BINARY_DIR}/package/$<CONFIG>/ocr"
    COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_BINARY_DIR}/package/$<CONFIG>/ocr/tessdata"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "$<TARGET_FILE:ocr>"
        "${CMAKE_BINARY_DIR}/package/$<CONFIG>/ocr/$<TARGET_FILE_NAME:ocr>"
    ${OCR_PACKAGE_TESSDATA_COMMANDS}
    COMMAND ${CMAKE_COMMAND} -E rm -f "${CMAKE_BINARY_DIR}/ocr.zip"
    COMMAND ${CMAKE_COMMAND} -E chdir "${CMAKE_BINARY_DIR}/package/$<CONFIG>/ocr"
        ${CMAKE_COMMAND} -E tar
            cf "${CMAKE_BINARY_DIR}/ocr.zip"
            --format=zip
            "$<TARGET_FILE_NAME:ocr>"
            tessdata
    VERBATIM)
