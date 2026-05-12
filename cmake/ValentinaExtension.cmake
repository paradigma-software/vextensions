function(add_valentina_extension)
    set(options)
    set(one_value_args NAME)
    set(multi_value_args
        SOURCES
        INCLUDE_DIRECTORIES
        LINK_LIBRARIES
        DATA_DIRS)

    cmake_parse_arguments(EXT
        "${options}"
        "${one_value_args}"
        "${multi_value_args}"
        ${ARGN})

    if(NOT EXT_NAME)
        message(FATAL_ERROR "add_valentina_extension requires NAME")
    endif()

    if(NOT EXT_SOURCES)
        message(FATAL_ERROR "add_valentina_extension(${EXT_NAME}) requires SOURCES")
    endif()

    add_library(${EXT_NAME} MODULE ${EXT_SOURCES})
    target_compile_features(${EXT_NAME} PRIVATE cxx_std_20)
    target_include_directories(${EXT_NAME}
        PRIVATE
            ${PROJECT_SOURCE_DIR}/include
            ${EXT_INCLUDE_DIRECTORIES})
    target_link_libraries(${EXT_NAME} PRIVATE ${EXT_LINK_LIBRARIES})

    foreach(data_dir IN LISTS EXT_DATA_DIRS)
        if(NOT IS_DIRECTORY ${data_dir})
            message(FATAL_ERROR "Missing data directory: ${data_dir}")
        endif()

        get_filename_component(data_dir_name ${data_dir} NAME)
        file(GLOB_RECURSE data_files CONFIGURE_DEPENDS
            "${data_dir}/*")

        foreach(data_file IN LISTS data_files)
            if(IS_DIRECTORY ${data_file})
                continue()
            endif()

            file(RELATIVE_PATH data_file_rel ${data_dir} ${data_file})
            get_filename_component(data_file_rel_dir ${data_file_rel} DIRECTORY)
            if(data_file_rel_dir)
                list(APPEND make_data_dir_commands
                    COMMAND ${CMAKE_COMMAND} -E make_directory
                        "$<TARGET_FILE_DIR:${EXT_NAME}>/${data_dir_name}/${data_file_rel_dir}")
                list(APPEND make_package_data_dir_commands
                    COMMAND ${CMAKE_COMMAND} -E make_directory
                        "${CMAKE_BINARY_DIR}/package/$<CONFIG>/${EXT_NAME}/${data_dir_name}/${data_file_rel_dir}")
            endif()

            list(APPEND copy_data_commands
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                    ${data_file}
                    "$<TARGET_FILE_DIR:${EXT_NAME}>/${data_dir_name}/${data_file_rel}")
            list(APPEND package_data_commands
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                    ${data_file}
                    "${CMAKE_BINARY_DIR}/package/$<CONFIG>/${EXT_NAME}/${data_dir_name}/${data_file_rel}")
        endforeach()

        list(APPEND make_data_dir_commands
            COMMAND ${CMAKE_COMMAND} -E make_directory
                "$<TARGET_FILE_DIR:${EXT_NAME}>/${data_dir_name}")
        list(APPEND make_package_data_dir_commands
            COMMAND ${CMAKE_COMMAND} -E make_directory
                "${CMAKE_BINARY_DIR}/package/$<CONFIG>/${EXT_NAME}/${data_dir_name}")
        list(APPEND package_archive_entries ${data_dir_name})
    endforeach()

    add_custom_command(
        TARGET ${EXT_NAME} POST_BUILD
        ${make_data_dir_commands}
        ${copy_data_commands}
        COMMAND ${CMAKE_COMMAND} -E rm -rf "${CMAKE_BINARY_DIR}/package/$<CONFIG>/${EXT_NAME}"
        COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_BINARY_DIR}/package/$<CONFIG>/${EXT_NAME}"
        ${make_package_data_dir_commands}
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "$<TARGET_FILE:${EXT_NAME}>"
            "${CMAKE_BINARY_DIR}/package/$<CONFIG>/${EXT_NAME}/$<TARGET_FILE_NAME:${EXT_NAME}>"
        ${package_data_commands}
        COMMAND ${CMAKE_COMMAND} -E rm -f "${CMAKE_BINARY_DIR}/${EXT_NAME}.zip"
        COMMAND ${CMAKE_COMMAND} -E chdir "${CMAKE_BINARY_DIR}/package/$<CONFIG>/${EXT_NAME}"
            ${CMAKE_COMMAND} -E tar
                cf "${CMAKE_BINARY_DIR}/${EXT_NAME}.zip"
                --format=zip
                "$<TARGET_FILE_NAME:${EXT_NAME}>"
                ${package_archive_entries}
        VERBATIM)

    if(NOT TARGET package_extensions)
        add_custom_target(package_extensions)
    endif()
    add_dependencies(package_extensions ${EXT_NAME})
endfunction()
