# ########################################################################
# Copyright 2019-2024 Advanced Micro Devices, Inc.
# ########################################################################

cmake_minimum_required(VERSION 3.10.2 FATAL_ERROR)

# Install prefix
if(WIN32)
  set(CMAKE_INSTALL_PREFIX ${PROJECT_BINARY_DIR}/package CACHE PATH "Install path prefix, prepended onto install directories")
else()
  set(CMAKE_INSTALL_PREFIX "/opt/rocm" CACHE PATH "Install path prefix, prepended onto install directories")
endif()

# Thrust project
project(rocthrust LANGUAGES CXX)

#Adding CMAKE_PREFIX_PATH
list( APPEND CMAKE_PREFIX_PATH /opt/rocm/llvm /opt/rocm ${ROCM_PATH} )

# CMake modules
list(APPEND CMAKE_MODULE_PATH
  ${CMAKE_CURRENT_SOURCE_DIR}/cmake
  ${ROCM_PATH}/lib/cmake/hip /opt/rocm/lib/cmake/hip # FindHIP.cmake
  ${HIP_PATH}/cmake /opt/rocm/hip/cmake # FindHIP.cmake
)

# Set a default build type if none was specified
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to 'Release' as none was specified.")
  set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build." FORCE)
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "" "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE CACHE BOOL "Add paths to linker search and installed rpath")

# rocm-cmake contains common cmake code for rocm projects to help
# setup and install
include( cmake/FindROCMCmake.cmake )
include( ROCMSetupVersion )
include( ROCMCreatePackage )
include( ROCMInstallTargets )
include( ROCMPackageConfigHelpers )
include( ROCMInstallSymlinks )
include( ROCMHeaderWrapper )
include( ROCMCheckTargetIds )
include( ROCMClients )

# Use target ID syntax if supported for GPU_TARGETS
if (NOT DEFINED AMDGPU_TARGETS)
  set(GPU_TARGETS "all" CACHE STRING "GPU architectures to compile for")
else()
  set(GPU_TARGETS "${AMDGPU_TARGETS}" CACHE STRING "GPU architectures to compile for")
endif()
set_property(CACHE GPU_TARGETS PROPERTY STRINGS "all")

if(GPU_TARGETS STREQUAL "all")
  if(BUILD_ADDRESS_SANITIZER)
    # ASAN builds require xnack
    rocm_check_target_ids(DEFAULT_AMDGPU_TARGETS
      TARGETS "gfx908:xnack+;gfx90a:xnack+;gfx940:xnack+;gfx941:xnack+;gfx942:xnack+"
    )
  else()
    rocm_check_target_ids(DEFAULT_AMDGPU_TARGETS
      TARGETS "gfx803;gfx900:xnack-;gfx906:xnack-;gfx908:xnack-;gfx90a:xnack-;gfx90a:xnack+;gfx940;gfx941;gfx942;gfx1030;gfx1100;gfx1101;gfx1102;gfx1200;gfx1201"
    )
  endif()
  set(GPU_TARGETS "${DEFAULT_AMDGPU_TARGETS}" CACHE STRING "GPU architectures to compile for" FORCE)
endif()

# Get dependencies
include(cmake/Dependencies.cmake)

# Verify that supported compilers are used
if (NOT WIN32)
  include(cmake/VerifyCompiler.cmake)
endif()
# Build options
# Disable -Werror
option(DISABLE_WERROR "Disable building with Werror" ON)
option(BUILD_TEST "Build tests" OFF)
option(BUILD_EXAMPLES "Build examples" OFF)
option(BUILD_BENCHMARKS "Build benchmarks" OFF)
option(DOWNLOAD_ROCPRIM "Download rocPRIM and do not search for rocPRIM package" OFF)
option(BUILD_ADDRESS_SANITIZER "Build with address sanitizer enabled" OFF)
option(ENABLE_UPSTREAM_TESTS "Enable upstream tests" OFF)
#Set the header wrapper OFF by default.
option(BUILD_FILE_REORG_BACKWARD_COMPATIBILITY "Build with file/folder reorg with backward compatibility enabled" OFF)

set(RNG_SEED_COUNT 0 CACHE STRING "Number of true random sequences to test each input size for")
set(PRNG_SEEDS 1 CACHE STRING "Seeds of pseudo random sequences to test each input size for")

set(THRUST_HOST_SYSTEM_OPTIONS CPP OMP TBB)
set(THRUST_HOST_SYSTEM CPP CACHE STRING "The device backend to target.")
set_property(
  CACHE THRUST_HOST_SYSTEM
  PROPERTY STRINGS ${THRUST_HOST_SYSTEM_OPTIONS}
)

if (NOT THRUST_HOST_SYSTEM IN_LIST THRUST_HOST_SYSTEM_OPTIONS)
  message(
    FATAL_ERROR
    "THRUST_HOST_SYSTEM must be one of ${THRUST_HOST_SYSTEM_OPTIONS}"
  )
endif ()

# Set CXX flags
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(DISABLE_WERROR)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra ")
else()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror")
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

if (WIN32)
  add_compile_options(-xhip)
  add_compile_definitions(THRUST_IGNORE_DEPRECATED_CPP_DIALECT)
endif()

# Address Sanitizer
if(BUILD_ADDRESS_SANITIZER)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -shared-libasan")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=address -shared-libasan")
  add_link_options(-fuse-ld=lld)
endif()

# Setup VERSION
rocm_setup_version(VERSION "3.0.1")

# Print configuration summary
include(cmake/Summary.cmake)
print_configuration_summary()

# Thrust (with HIP backend)
add_subdirectory(thrust)

if(BUILD_TEST OR BUILD_BENCHMARKS)
  rocm_package_setup_component(clients)
endif()

# Tests
if(BUILD_TEST)
  rocm_package_setup_client_component(tests)
  if (ENABLE_UPSTREAM_TESTS)
    enable_testing()
  endif()
  # We still want the testing to be compiled to catch some errors
  #TODO: Get testing folder working with HIP on Windows
  if (NOT WIN32)
    add_subdirectory(testing)
  endif()
  enable_testing()
  add_subdirectory(test)
endif()

# Examples
if(BUILD_EXAMPLES)
  add_subdirectory(examples)
endif()

# Benchmarks
if(BUILD_BENCHMARKS)
  add_subdirectory(internal/benchmark)
endif()

#Create header wrapper for backward compatibility
if(BUILD_FILE_REORG_BACKWARD_COMPATIBILITY AND NOT WIN32)
  rocm_wrap_header_dir(
      ${CMAKE_SOURCE_DIR}/thrust
      PATTERNS "*.h" "*.inl" "*.cuh" "*.hpp"
      HEADER_LOCATION include/thrust
      GUARDS SYMLINK WRAPPER
      WRAPPER_LOCATIONS rocthrust/${CMAKE_INSTALL_INCLUDEDIR}/thrust
      OUTPUT_LOCATIONS rocthrust/wrapper/include/thrust
  )
endif( )

set(THRUST_OPTIONS_DEBUG ${THRUST_OPTIONS_WARNINGS})
set(THRUST_OPTIONS_RELEASE ${THRUST_OPTIONS_WARNINGS})

# Package
set(CPACK_DEBIAN_ARCHIVE_TYPE "gnutar")

rocm_package_add_deb_dependencies(DEPENDS "rocprim-dev >= 2.10.1")
rocm_package_add_rpm_dependencies(DEPENDS "rocprim-devel >= 2.10.1")
set(CPACK_DEBIAN_PACKAGE_CONFLICTS "hip-thrust, thrust")
set(CPACK_RPM_PACKAGE_CONFLICTS "hip-thrust, thrust")
set(CPACK_DEBIAN_DEVEL_PACKAGE_PROVIDES "hipstdpar")
set(CPACK_RPM_DEVEL_PACKAGE_PROVIDES "hipstdpar")

set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
set(CPACK_RPM_PACKAGE_LICENSE "ASL 2.0")

# if(NOT CPACK_PACKAGING_INSTALL_PREFIX)
#   set(CPACK_PACKAGING_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")
# endif()

set(CPACK_RPM_EXCLUDE_FROM_AUTO_FILELIST_ADDITION "\${CPACK_PACKAGING_INSTALL_PREFIX}" )

rocm_create_package(
  NAME rocthrust
  DESCRIPTION "Radeon Open Compute Thrust library"
  MAINTAINER "rocthrust-maintainer@amd.com"
  HEADER_ONLY
)
