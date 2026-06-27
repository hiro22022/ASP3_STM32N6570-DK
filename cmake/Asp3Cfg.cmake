# ASP3 configurator (cfg.rb) three-pass integration.

function(asp3_cfg_setup target kernel_lib)
    set(CFG_SCRIPT "${ASP3_SRCDIR}/cfg/cfg.rb" CACHE FILEPATH "ASP3 configurator")
    set(CFG_KERNEL_ARGS
        --kernel asp
        --api-table "${ASP3_SRCDIR}/kernel/kernel_api.def"
        --symval-table "${ASP3_SRCDIR}/kernel/kernel_sym.def"
    )
    if(ASP3_CFG_EXTRA_TABS)
        list(APPEND CFG_KERNEL_ARGS ${ASP3_CFG_EXTRA_TABS})
    endif()

    set(CFG_GEN_DIR "${CMAKE_BINARY_DIR}/cfg_gen")
    file(MAKE_DIRECTORY "${CFG_GEN_DIR}")

    set(CFG1_C "${CFG_GEN_DIR}/cfg1_out.c")
    set(CFG1_DB "${CFG_GEN_DIR}/cfg1_out.db")
    set(CFG1_ELF "${CMAKE_BINARY_DIR}/cfg1_out.elf")
    set(CFG1_SYMS "${CFG_GEN_DIR}/cfg1_out.syms")
    set(CFG1_SREC "${CFG_GEN_DIR}/cfg1_out.srec")
    set(KERNEL_CFG_H "${CFG_GEN_DIR}/kernel_cfg.h")
    set(KERNEL_CFG_C "${CFG_GEN_DIR}/kernel_cfg.c")
    set(CFG2_DB "${CFG_GEN_DIR}/cfg2_out.db")

    set(_cfg_includes "")
    foreach(_inc ${ASP3_CFG_INCLUDE_DIRS})
        list(APPEND _cfg_includes "-I${_inc}")
    endforeach()

    # Pass 1: cfg1_out.c and cfg1_out.db
    add_custom_command(
        OUTPUT "${CFG1_C}" "${CFG1_DB}"
        COMMAND ${RUBY_EXECUTABLE} ${CFG_SCRIPT} --pass 1 ${CFG_KERNEL_ARGS}
            ${_cfg_includes}
            -M "${CFG_GEN_DIR}/cfg1_out.c.d"
            "${ASP3_TARGET_KERNEL_CFG}"
            "${ASP3_APPL_CFG}"
        DEPENDS
            "${ASP3_APPL_CFG}"
            "${ASP3_TARGET_KERNEL_CFG}"
            "${TECS_GEN_DIR}/tecsgen.cfg"
        WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
        COMMENT "cfg.rb pass 1"
    )

    add_executable(cfg1_out EXCLUDE_FROM_ALL "${CFG1_C}")
    target_link_libraries(cfg1_out PRIVATE ${kernel_lib} c m gcc)
    target_include_directories(cfg1_out PRIVATE
        ${ASP3_CFG_INCLUDE_DIRS}
        "${CFG_GEN_DIR}"
        "${TECS_GEN_DIR}"
    )
    target_compile_options(cfg1_out PRIVATE ${ASP3_TARGET_COMPILE_OPTIONS})
    target_compile_definitions(cfg1_out PRIVATE ${ASP3_TARGET_COMPILE_DEFINITIONS})
    target_link_options(cfg1_out PRIVATE ${ASP3_TARGET_LINK_OPTIONS})
    set_target_properties(cfg1_out PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
        OUTPUT_NAME "cfg1_out"
    )
    add_dependencies(cfg1_out ${kernel_lib})

    add_custom_command(
        OUTPUT "${CFG1_SYMS}" "${CFG1_SREC}"
        COMMAND ${CMAKE_NM} -n "$<TARGET_FILE:cfg1_out>" > "${CFG1_SYMS}"
        COMMAND ${CMAKE_OBJCOPY} -O srec -S "$<TARGET_FILE:cfg1_out>" "${CFG1_SREC}"
        DEPENDS cfg1_out
        COMMENT "Extract cfg1_out symbols"
    )

    # Pass 2: kernel_cfg.h / kernel_cfg.c
    add_custom_command(
        OUTPUT "${KERNEL_CFG_H}" "${KERNEL_CFG_C}" "${CFG2_DB}"
        COMMAND ${RUBY_EXECUTABLE} ${CFG_SCRIPT} --pass 2 ${CFG_KERNEL_ARGS}
            ${_cfg_includes}
            -T "${ASP3_TARGET_KERNEL_TRB}"
        DEPENDS "${CFG1_DB}" "${CFG1_SYMS}" "${CFG1_SREC}"
        WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
        COMMENT "cfg.rb pass 2"
    )

    if(ASP3_TARGET_OFFSET_TRB)
        set(OFFSET_H "${CFG_GEN_DIR}/offset.h")
        add_custom_command(
            OUTPUT "${OFFSET_H}"
            COMMAND ${RUBY_EXECUTABLE} ${CFG_SCRIPT} --pass 2 -O ${CFG_KERNEL_ARGS}
                ${_cfg_includes}
                -T "${ASP3_TARGET_OFFSET_TRB}"
                --rom-symbol "${CFG1_SYMS}"
                --rom-image "${CFG1_SREC}"
            DEPENDS "${CFG1_DB}" "${CFG1_SYMS}" "${CFG1_SREC}"
            WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
            COMMENT "cfg.rb pass 2 offset.h"
        )
        add_custom_target(asp3_offset DEPENDS "${OFFSET_H}")
        add_dependencies(${kernel_lib} asp3_offset)
    endif()

    target_sources(${target} PRIVATE "${KERNEL_CFG_C}")
    target_include_directories(${target} PRIVATE "${CFG_GEN_DIR}")
    target_compile_definitions(${target} PRIVATE TOPPERS_CB_TYPE_ONLY)

    add_custom_target(asp3_kernel_cfg DEPENDS "${KERNEL_CFG_H}" "${KERNEL_CFG_C}")
    add_dependencies(${target} asp3_kernel_cfg)
    add_dependencies(${kernel_lib} asp3_kernel_cfg)
    if(ASP3_TARGET_OFFSET_TRB)
        add_dependencies(${target} asp3_offset)
    endif()

    # Pass 3: configuration check after final link
    add_custom_command(
        OUTPUT "${CMAKE_BINARY_DIR}/check.timestamp"
        COMMAND ${CMAKE_NM} -n "$<TARGET_FILE:${target}>" > "${CMAKE_BINARY_DIR}/asp.syms"
        COMMAND ${CMAKE_OBJCOPY} -O srec -S "$<TARGET_FILE:${target}>" "${CMAKE_BINARY_DIR}/asp.srec"
        COMMAND ${RUBY_EXECUTABLE} ${CFG_SCRIPT} --pass 3 ${CFG_KERNEL_ARGS}
            -O ${_cfg_includes}
            -T "${ASP3_TARGET_CHECK_TRB}"
            --rom-symbol "${CMAKE_BINARY_DIR}/asp.syms"
            --rom-image "${CMAKE_BINARY_DIR}/asp.srec"
        COMMAND ${CMAKE_COMMAND} -E touch "${CMAKE_BINARY_DIR}/check.timestamp"
        DEPENDS ${target}
        COMMENT "cfg.rb pass 3 configuration check"
    )
    add_custom_target(asp3_check DEPENDS "${CMAKE_BINARY_DIR}/check.timestamp")
endfunction()
