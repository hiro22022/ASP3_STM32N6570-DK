cmake_minimum_required(VERSION 3.20)

project(asp3_tecs_cmake LANGUAGES C ASM)

set(ASP3_SRCDIR "${CMAKE_CURRENT_SOURCE_DIR}" CACHE PATH "ASP3 source root")

option(ASP3_OMIT_TECS "Build without TECS" OFF)
option(ASP3_ENABLE_TRACE "Enable trace log sample" OFF)

set(ASP3_TARGET "stm32n6570_dk" CACHE STRING "ASP3 target board name")
set(ASP3_APPL_NAME "sample1_n657" CACHE STRING "Application name")
set(ASP3_APPL_CFG "${ASP3_SRCDIR}/sample/${ASP3_APPL_NAME}.cfg" CACHE FILEPATH "System configuration file")
set(ASP3_APPL_CDL "${ASP3_SRCDIR}/sample/${ASP3_APPL_NAME}.cdl" CACHE FILEPATH "TECS CDL file")

find_program(RUBY_EXECUTABLE ruby REQUIRED)

list(APPEND CMAKE_MODULE_PATH "${ASP3_SRCDIR}/cmake")
include(Asp3Tecs)
include(Asp3Kernel)
include(Asp3Cfg)

if(ASP3_TARGET STREQUAL "stm32n6570_dk")
    include(targets/stm32n6570_dk)
else()
    message(FATAL_ERROR "Unsupported ASP3_TARGET: ${ASP3_TARGET}")
endif()

set(ASP3_GLOBAL_INCLUDE_DIRS
    "${CMAKE_BINARY_DIR}"
    "${ASP3_SRCDIR}/include"
    "${ASP3_SRCDIR}"
    "${ASP3_SRCDIR}/tecs_kernel"
    "${ASP3_SRCDIR}/syssvc"
    "${ASP3_SRCDIR}/sample"
)

set(ASP3_GLOBAL_COMPILE_DEFINITIONS "")
if(ASP3_OMIT_TECS)
    list(APPEND ASP3_GLOBAL_COMPILE_DEFINITIONS TOPPERS_OMIT_TECS)
endif()
if(ASP3_ENABLE_TRACE)
    list(APPEND ASP3_GLOBAL_COMPILE_DEFINITIONS TOPPERS_ENABLE_TRACE)
endif()

set(TECS_GEN_DIR "${CMAKE_BINARY_DIR}/gen" CACHE PATH "TECS generated sources")

set(ASP3_CFG_INCLUDE_DIRS
    ${ASP3_GLOBAL_INCLUDE_DIRS}
    ${ASP3_TARGET_INCLUDE_DIRS}
    ${ASP3_ARCH_INCLUDE_DIRS}
    "${TECS_GEN_DIR}"
)

if(NOT ASP3_OMIT_TECS)
    asp3_tecs_configure("${ASP3_APPL_CDL}" "${TECS_GEN_DIR}")
endif()

asp3_add_kernel_library(kernel)

add_executable(asp
    ${ASP3_STARTUP_OBJECTS}
    "${ASP3_SRCDIR}/sample/${ASP3_APPL_NAME}.c"
    "${ASP3_SRCDIR}/library/log_output.c"
    "${ASP3_SRCDIR}/library/vasyslog.c"
    "${ASP3_SRCDIR}/library/t_perror.c"
    "${ASP3_SRCDIR}/library/strerror.c"
    "${ASP3_SRCDIR}/tecs_kernel/init_tecs.c"
    ${ASP3_TARGET_APPL_SOURCES}
)

target_link_libraries(asp PRIVATE kernel c m gcc)

target_include_directories(asp PRIVATE
    ${ASP3_GLOBAL_INCLUDE_DIRS}
    ${ASP3_TARGET_INCLUDE_DIRS}
    ${ASP3_ARCH_INCLUDE_DIRS}
    "${ASP3_SRCDIR}/tecs_kernel"
    "${ASP3_SRCDIR}/sample"
    "${ASP3_SRCDIR}/library"
)

target_compile_options(asp PRIVATE ${ASP3_TARGET_COMPILE_OPTIONS})
target_compile_definitions(asp PRIVATE ${ASP3_TARGET_COMPILE_DEFINITIONS})
target_link_options(asp PRIVATE ${ASP3_TARGET_LINK_OPTIONS})

if(NOT ASP3_OMIT_TECS)
    asp3_tecs_apply_manifest(asp)
endif()

if(NOT ASP3_OMIT_TECS)
    asp3_cfg_setup(asp kernel)
    add_dependencies(asp asp3_check)
endif()

# Assembly sources
set_source_files_properties(${ASP3_STARTUP_OBJECTS} PROPERTIES LANGUAGE ASM)

message(STATUS "ASP3 CMake: target=${ASP3_TARGET} appl=${ASP3_APPL_NAME}")
message(STATUS "ASP3 CMake: TECS_GEN_DIR=${TECS_GEN_DIR}")
message(STATUS "ASP3 CMake: see cmake/DesignDecisions.txt for design rationale")
