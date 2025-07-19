cmake_minimum_required(VERSION 3.22)

#
# This file is generated only once,
# and is not re-generated if converter is called multiple times.
#
# User is free to modify the file as much as necessary
#

# Setup compiler settings
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)


# Define the build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

# Set the project name
set(CMAKE_PROJECT_NAME KRTOS)

# Include toolchain file
include("cmake/gcc-arm-none-eabi.cmake")

# Enable compile command to ease indexing with e.g. clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

# Enable CMake support for ASM and C languages
enable_language(C ASM)

# Core project settings
project(${CMAKE_PROJECT_NAME})
message("Build type: " ${CMAKE_BUILD_TYPE})

# Create an executable object type
add_executable(${CMAKE_PROJECT_NAME})

# Disable hardware FPU, use software floating point
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mfloat-abi=soft")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mfloat-abi=soft")
set(CMAKE_ASM_FLAGS "${CMAKE_ASM_FLAGS} -mfloat-abi=soft")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -mfloat-abi=soft")

# Add STM32CubeMX generated sources
add_subdirectory(cmake/stm32cubemx)

# Specify QP port for STM32 Cortex-M
set(QP_PORT "arm-cm")

# Add QP/C framework
# Create QPC static library
add_library(qpc STATIC
    lib/qpc/src/qf/qep_hsm.c
    lib/qpc/src/qf/qep_msm.c
    lib/qpc/src/qf/qf_act.c
    lib/qpc/src/qf/qf_actq.c
    lib/qpc/src/qf/qf_defer.c
    lib/qpc/src/qf/qf_dyn.c
    lib/qpc/src/qf/qf_mem.c
    lib/qpc/src/qf/qf_ps.c
    lib/qpc/src/qf/qf_qact.c
    lib/qpc/src/qf/qf_qeq.c
    lib/qpc/src/qf/qf_qmact.c
    lib/qpc/src/qf/qf_time.c

    lib/qpc/ports/arm-cm/qxk/gnu/qxk_port.c

    # Include qxk only if you're using it:
    lib/qpc/src/qxk/qxk.c
    lib/qpc/src/qxk/qxk_mutex.c
    lib/qpc/src/qxk/qxk_sema.c
    lib/qpc/src/qxk/qxk_xthr.c
    lib/qpc/3rd_party/uC-OS2/Source/os_core.c
    lib/qpc/3rd_party/uC-OS2/Source/os_flag.c
    lib/qpc/3rd_party/uC-OS2/Source/os_mbox.c
    lib/qpc/3rd_party/uC-OS2/Source/os_mem.c
    lib/qpc/3rd_party/uC-OS2/Source/os_mutex.c
    lib/qpc/3rd_party/uC-OS2/Source/os_q.c
    lib/qpc/3rd_party/uC-OS2/Source/os_sem.c
    lib/qpc/3rd_party/uC-OS2/Source/os_task.c
    lib/qpc/3rd_party/uC-OS2/Source/os_time.c
    lib/qpc/3rd_party/uC-OS2/Source/os_tmr.c
    lib/qpc/3rd_party/uC-OS2/Ports/ARM-Cortex-M/ARMv7-M/os_cpu_c.c
    lib/qpc/3rd_party/uC-OS2/Ports/ARM-Cortex-M/ARMv7-M/GNU/os_cpu_a.s
    lib/qpc/3rd_party/uC-OS2/Ports/ARM-Cortex-M/ARMv7-M/GNU/os_dbg.c
)

target_include_directories(qpc PUBLIC
    ${CMAKE_SOURCE_DIR}/lib/qpc/include
    ${CMAKE_SOURCE_DIR}/lib/qpc/src
    ${CMAKE_SOURCE_DIR}/lib/qpc/ports/arm-cm/qxk/gnu
    ${CMAKE_SOURCE_DIR}/lib_modified
    ${CMAKE_SOURCE_DIR}/lib/qpc/3rd_party/uC-OS2/Source
    ${CMAKE_SOURCE_DIR}/lib/qpc/3rd_party/uC-OS2/Ports/ARM-Cortex-M/ARMv7-M/GNU
)


# Link directories setup
target_link_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined library search paths
)

# Add sources to executable
target_sources(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user sources here
)

# Add include paths
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined include paths
)

# Add project symbols (macros)
target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined symbols
)

# Add linked libraries
target_link_libraries(${CMAKE_PROJECT_NAME}
    stm32cubemx
    qpc
    # Add user defined libraries
)
