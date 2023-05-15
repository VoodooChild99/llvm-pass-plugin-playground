cmake_minimum_required(VERSION 3.13.4)

set(LLVM_CONFIG "llvm-config")
if(DEFINED ENV{LLVM_CONFIG})
  message("LLVM_CONFIG: $ENV{LLVM_CONFIG}")
  set(LLVM_CONFIG $ENV{LLVM_CONFIG})
else()
  message("LLVM_CONFIG is not set, default to: llvm-config")
endif()

execute_process(
  COMMAND bash -c "${LLVM_CONFIG} --cmakedir 2>/dev/null"
  RESULT_VARIABLE llvm_config_success
  OUTPUT_VARIABLE llvm_cmake_dir
  OUTPUT_STRIP_TRAILING_WHITESPACE
)
if(llvm_config_success AND NOT llvm_config_success EQUAL 0)
  message(FATAL_ERROR "Failed to execute ${LLVM_CONFIG} --cmakedir")
endif()

execute_process(
  COMMAND bash -c "${LLVM_CONFIG} --version 2>/dev/null | sed 's/git//' | sed 's/svn//'"
  RESULT_VARIABLE llvm_config_success
  OUTPUT_VARIABLE llvm_version
  OUTPUT_STRIP_TRAILING_WHITESPACE
)
if(llvm_config_success AND NOT llvm_config_success EQUAL 0)
  message(FATAL_ERROR "Failed to get LLVM version")
endif()

execute_process(
  COMMAND bash -c "${LLVM_CONFIG} --version 2>/dev/null | sed 's/\\..*//'"
  RESULT_VARIABLE llvm_config_success
  OUTPUT_VARIABLE llvm_version_major
  OUTPUT_STRIP_TRAILING_WHITESPACE
)
if(llvm_config_success AND NOT llvm_config_success EQUAL 0)
  message(FATAL_ERROR "Failed to get LLVM major version")
endif()

execute_process(
  COMMAND bash -c "${LLVM_CONFIG} --bindir 2>/dev/null"
  RESULT_VARIABLE llvm_config_success
  OUTPUT_VARIABLE llvm_bindir
  OUTPUT_STRIP_TRAILING_WHITESPACE
)
if(llvm_config_success AND NOT llvm_config_success EQUAL 0)
  message(FATAL_ERROR "Failed to get LLVM binary directory")
endif()
set(CMAKE_C_COMPILER "${llvm_bindir}/clang")
set(CMAKE_CXX_COMPILER "${llvm_bindir}/clang++")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

message("LLVM version: ${llvm_version}")

project(llvm-pass-plugin-playground)
#===============================================================================
# 2. LOAD LLVM CONFIGURATION
#    For more: http://llvm.org/docs/CMake.html#embedding-llvm-in-your-project
#===============================================================================
# Add the location of LLVMConfig.cmake to CMake search paths (so that
# find_package can locate it)
# Note: On Fedora, when using the pre-compiled binaries installed with `dnf`,
# LLVMConfig.cmake is located in "/usr/lib64/cmake/llvm". But this path is
# among other paths that will be checked by default when using
# `find_package(llvm)`. So there's no need to add it here.
list(APPEND CMAKE_PREFIX_PATH "${llvm_cmake_dir}/llvm/")

find_package(LLVM ${llvm_version_major} REQUIRED CONFIG)

# Another sanity check
if(NOT "${llvm_version_major}" VERSION_EQUAL "${LLVM_VERSION_MAJOR}")
  message(FATAL_ERROR "Found LLVM ${LLVM_VERSION_MAJOR}, but need LLVM ${llvm_version_major}")
endif()

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${llvm_cmake_dir}/llvm")

message("LLVM STATUS:
  Definitions ${LLVM_DEFINITIONS}
  Includes    ${LLVM_INCLUDE_DIRS}
  Libraries   ${LLVM_LIBRARY_DIRS}
  Targets     ${LLVM_TARGETS_TO_BUILD}"
)

# Set the LLVM header and library paths
include_directories(SYSTEM ${LLVM_INCLUDE_DIRS})
link_directories(${LLVM_LIBRARY_DIRS})
add_definitions(${LLVM_DEFINITIONS})

#===============================================================================
# 3. LLVM-TUTOR BUILD CONFIGURATION
#===============================================================================
# Use the same C++ standard as LLVM does
set(CMAKE_CXX_STANDARD 17 CACHE STRING "")

# Build type
if (NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug CACHE
      STRING "Build type (default Debug):" FORCE)
endif()

# Compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall\
  -fdiagnostics-color=always")

# LLVM is normally built without RTTI. Be consistent with that.
if(NOT LLVM_ENABLE_RTTI)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-rtti")
endif()

# -fvisibility-inlines-hidden is set when building LLVM and on Darwin warnings
# are triggered if llvm-tutor is built without this flag (though otherwise it
# builds fine). For consistency, add it here too.
include(CheckCXXCompilerFlag)
check_cxx_compiler_flag("-fvisibility-inlines-hidden" SUPPORTS_FVISIBILITY_INLINES_HIDDEN_FLAG)
if (${SUPPORTS_FVISIBILITY_INLINES_HIDDEN_FLAG} EQUAL "1")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility-inlines-hidden")
endif()

# Set the build directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib")

#===============================================================================
# 4. ADD SUB-TARGETS
# Doing this at the end so that all definitions and link/include paths are
# available for the sub-projects.
#===============================================================================
add_subdirectory(lib)