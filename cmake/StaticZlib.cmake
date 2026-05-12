include_guard(GLOBAL)

include(FetchContent)

function(static_zlib_write_find_package_redirect package content)
    string(TOLOWER "${package}" package_lower)
    file(WRITE "${CMAKE_FIND_PACKAGE_REDIRECTS_DIR}/${package}Config.cmake" "${content}")
    file(WRITE "${CMAKE_FIND_PACKAGE_REDIRECTS_DIR}/${package_lower}-config.cmake" "${content}")
endfunction()

set(ZLIB_BUILD_EXAMPLES OFF CACHE BOOL "Do not build zlib examples" FORCE)

FetchContent_Declare(
    zlib
    GIT_REPOSITORY https://github.com/madler/zlib.git
    GIT_TAG        v1.3.1
    GIT_SHALLOW    TRUE)

FetchContent_MakeAvailable(zlib)

if(NOT TARGET ZLIB::ZLIB)
    add_library(ZLIB::ZLIB INTERFACE IMPORTED GLOBAL)
    set_target_properties(ZLIB::ZLIB PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${zlib_SOURCE_DIR};${zlib_BINARY_DIR}"
        INTERFACE_LINK_LIBRARIES zlibstatic)
endif()

static_zlib_write_find_package_redirect(ZLIB
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
