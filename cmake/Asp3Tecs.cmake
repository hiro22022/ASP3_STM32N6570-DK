# TECS (tecsgen) integration for ASP3 CMake builds.

function(asp3_tecs_run_generator)
    set(options "")
    set(oneValueArgs CDL_FILE GEN_DIR)
    set(multiValueArgs INCLUDE_DIRS COMPILE_DEFINITIONS)
    cmake_parse_arguments(TECS "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if(NOT TECS_CDL_FILE)
        message(FATAL_ERROR "asp3_tecs_run_generator: CDL_FILE is required")
    endif()
    if(NOT TECS_GEN_DIR)
        message(FATAL_ERROR "asp3_tecs_run_generator: GEN_DIR is required")
    endif()

    file(MAKE_DIRECTORY "${TECS_GEN_DIR}")

    set(_tecs_includes "")
    foreach(_inc ${TECS_INCLUDE_DIRS})
        list(APPEND _tecs_includes "-I${_inc}")
    endforeach()

    set(_tecs_defs "")
    foreach(_def ${TECS_COMPILE_DEFINITIONS})
        list(APPEND _tecs_defs "-D${_def}")
    endforeach()

    set(_cpp_cmd
        "${CMAKE_C_COMPILER}"
        ${_tecs_defs}
        ${_tecs_includes}
        -DTECSGEN
        -E
    )
    string(REPLACE ";" " " _cpp_cmd_str "${_cpp_cmd}")

    set(_tecsgen_cmd
        ${RUBY_EXECUTABLE}
        "${ASP3_SRCDIR}/tecsgen/tecsgen.rb"
        "${TECS_CDL_FILE}"
        -R ${_tecs_includes}
        --cpp "${_cpp_cmd_str}"
        -g "${TECS_GEN_DIR}"
    )

    execute_process(
        COMMAND ${_tecsgen_cmd}
        WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
        RESULT_VARIABLE _tecsgen_result
        OUTPUT_VARIABLE _tecsgen_out
        ERROR_VARIABLE _tecsgen_err
    )
    if(NOT _tecsgen_result EQUAL 0)
        message(FATAL_ERROR "tecsgen failed:\n${_tecsgen_out}\n${_tecsgen_err}")
    endif()

    set(_manifest "${TECS_GEN_DIR}/CMakeLists.tecsgen.cmake")
    if(NOT EXISTS "${_manifest}")
        message(FATAL_ERROR "tecsgen did not produce ${_manifest}")
    endif()
endfunction()

function(asp3_tecs_apply_manifest target)
    if(NOT TARGET ${target})
        message(FATAL_ERROR "asp3_tecs_apply_manifest: target ${target} not found")
    endif()

    set(_all_tecs_sources
        ${TECS_TECSGEN_SOURCES}
        ${TECS_PLUGIN_TECSGEN_SOURCES}
        ${TECS_PLUGIN_CELLTYPE_SOURCES}
        ${TECS_CELLTYPE_SOURCES}
        ${TECS_PLUGIN_EXTRA_SOURCES}
    )

    foreach(_src ${_all_tecs_sources})
        if(IS_ABSOLUTE "${_src}")
            set(_resolved "${_src}")
        elseif(_src MATCHES "^\\$\\{TECS_GEN_DIR\\}/")
            string(REPLACE "\${TECS_GEN_DIR}/" "" _rel "${_src}")
            set(_resolved "${TECS_GEN_DIR}/${_rel}")
        else()
            set(_resolved "${ASP3_SRCDIR}/${_src}")
        endif()
        if(EXISTS "${_resolved}")
            target_sources(${target} PRIVATE "${_resolved}")
        else()
            message(WARNING "TECS source not found (may be generated later): ${_resolved}")
            target_sources(${target} PRIVATE "${_resolved}")
        endif()
    endforeach()

    target_include_directories(${target} PRIVATE ${TECS_INCLUDE_DIRS})
    target_compile_definitions(${target} PRIVATE ${TECS_COMPILE_DEFINITIONS})

    if(TECS_LINK_OPTIONS)
        target_link_options(${target} PRIVATE ${TECS_LINK_OPTIONS})
    endif()
endfunction()

function(asp3_tecs_configure cdl_file gen_dir)
    asp3_tecs_run_generator(
        CDL_FILE "${cdl_file}"
        GEN_DIR "${gen_dir}"
        INCLUDE_DIRS ${ASP3_GLOBAL_INCLUDE_DIRS}
        COMPILE_DEFINITIONS ${ASP3_GLOBAL_COMPILE_DEFINITIONS}
    )

    set(TECS_GEN_DIR "${gen_dir}" CACHE INTERNAL "TECS generated file directory")
    include("${gen_dir}/CMakeLists.tecsgen.cmake")

    set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${cdl_file}")
    foreach(_cdl ${TECS_IMPORT_CDLS})
        if(EXISTS "${_cdl}")
            set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${_cdl}")
        endif()
    endforeach()
endfunction()
