# ============================================================================
# Modern C++ Project Template - CMake Build Configuration
# ============================================================================
#
# This CMakeLists.txt file configures the build process for a modern C++
# project with the following features:
#
# - C++20 standard support
# - Configurable version information
# - Support for tests, examples, benchmarks, and documentation
# - Compiler warnings and sanitizers for quality assurance
# - Package configuration for easy integration in other projects
#
# USAGE:
#   mkdir build && cd build
#   cmake ..
#   cmake --build .
#
# CONFIGURATION OPTIONS:
#   -DBUILD_SHARED_LIBS=ON|OFF     - Build shared libraries instead of static
#   -DBUILD_EXAMPLES=ON|OFF        - Build example programs
#   -DBUILD_TESTS=ON|OFF           - Build and enable tests
#   -DBUILD_BENCHMARKS=ON|OFF      - Build benchmarking programs
#   -DENABLE_COVERAGE=ON|OFF       - Enable code coverage reporting
#   -DENABLE_SANITIZERS=ON|OFF     - Enable sanitizers in debug builds
#   -DENABLE_PCH=ON|OFF            - Enable precompiled headers
#   -DENABLE_LTO=ON|OFF            - Enable Link Time Optimization
#
# ============================================================================

# Minimum required CMake version
cmake_minimum_required(VERSION 3.16)

# Project version control
# Change these values when releasing a new version
set(PROJECT_VERSION_MAJOR 0)    # Major version for breaking changes
set(PROJECT_VERSION_MINOR 1)    # Minor version for new features
set(PROJECT_VERSION_PATCH 0)    # Patch version for bug fixes
set(PROJECT_VERSION "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}")

# Define the project with metadata
project(cpp_project_template 
    VERSION ${PROJECT_VERSION}
    DESCRIPTION "Modern C++ Project Template"
    HOMEPAGE_URL "https://github.com/hun756/CPP-Starter-Template"
    LANGUAGES CXX C  # Languages used in the project
)

# Build configuration options
# These can be set using -D flags when running cmake
option(BUILD_SHARED_LIBS "Build shared libraries" OFF)
option(BUILD_EXAMPLES "Build example programs" ON)
option(BUILD_TESTS "Build tests" ON)
option(BUILD_BENCHMARKS "Build benchmark programs" OFF)
option(ENABLE_COVERAGE "Enable coverage reporting" OFF)
option(ENABLE_SANITIZERS "Enable sanitizers in debug builds" OFF)
option(ENABLE_PCH "Enable precompiled headers" OFF)
option(ENABLE_LTO "Enable Link Time Optimization" OFF)

# Set output directories for all build artifacts
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)  # Static libraries
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)  # Shared libraries
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)  # Executables

# C++ standard configuration
set(CMAKE_CXX_STANDARD 20)                # Use C++20
set(CMAKE_CXX_STANDARD_REQUIRED ON)       # Require C++20 support
set(CMAKE_CXX_EXTENSIONS OFF)             # Disable compiler extensions

# Include custom CMake modules
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(CompilerWarnings)    # Configure compiler warnings
include(Sanitizers)          # Configure sanitizers (ASan, UBSan, etc.)
include(StaticAnalyzers)     # Configure static analyzers
include(LTO)                 # Configure Link Time Optimization
include(Packaging)           # Configure packaging options

# Generate configuration header with version information
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/configs/Config.h.in"
    "${CMAKE_CURRENT_BINARY_DIR}/generated/configs/ProjectConfig.h"
)

# Global include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include        # Public headers
    ${CMAKE_CURRENT_BINARY_DIR}/generated      # Generated headers
)

# Find all source files for the library, excluding main.cpp
file(GLOB_RECURSE LIB_SOURCES "src/*.cpp")
list(FILTER LIB_SOURCES EXCLUDE REGEX ".*main\\.cpp$")

# Create the main library target
add_library(${PROJECT_NAME} ${LIB_SOURCES})

# Create an alias target with namespace
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

# Apply compiler warnings to the library
target_compile_warnings(${PROJECT_NAME} PRIVATE)

# Apply sanitizers if enabled
if(ENABLE_SANITIZERS)
    target_enable_sanitizers(${PROJECT_NAME})
endif()

# Configure precompiled headers if enabled
if(ENABLE_PCH)
    target_precompile_headers(${PROJECT_NAME} PRIVATE
        <vector>
        <string>
        <map>
        <memory>
    )
endif()

# Set library properties
set_target_properties(${PROJECT_NAME} PROPERTIES
    VERSION ${PROJECT_VERSION}                 # Library version
    SOVERSION ${PROJECT_VERSION_MAJOR}         # ABI version
    POSITION_INDEPENDENT_CODE ON               # Generate PIC code
    EXPORT_NAME ${PROJECT_NAME}                # Name when exporting
)

# Configure include directories for the library
target_include_directories(${PROJECT_NAME} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>       # When building
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/generated>     # When building
    $<INSTALL_INTERFACE:include>                                 # When installing
)

# Create the main executable
add_executable(${PROJECT_NAME}_exe src/main.cpp)
target_link_libraries(${PROJECT_NAME}_exe PRIVATE ${PROJECT_NAME})
set_target_properties(${PROJECT_NAME}_exe PROPERTIES OUTPUT_NAME ${PROJECT_NAME})

# Build examples if enabled
if(BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

# Build and configure tests if enabled
if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(test)
endif()

# Build documentation if Doxygen is available
find_package(Doxygen)
if(DOXYGEN_FOUND)
    add_subdirectory(docs)
endif()

# Build benchmarks if enabled
if(BUILD_BENCHMARKS)
    add_subdirectory(bench)
endif()

# ============================================================================
# Installation Configuration
# ============================================================================

# Include GNUInstallDirs to get standard installation directories
include(GNUInstallDirs)

# Install targets (library and executable)
install(TARGETS ${PROJECT_NAME} ${PROJECT_NAME}_exe
    EXPORT ${PROJECT_NAME}Targets
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}     # Executables
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}     # Shared libraries
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}     # Static libraries
)

# Install header files
install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

# Install generated configuration header
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/generated/configs/ProjectConfig.h"
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/configs)

# Install CMake targets file
install(EXPORT ${PROJECT_NAME}Targets
    FILE ${PROJECT_NAME}Targets.cmake
    NAMESPACE ${PROJECT_NAME}::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
)

# Configure and install package configuration files
include(CMakePackageConfigHelpers)
configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/configs/${PROJECT_NAME}Config.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
)

# Generate and install version information file
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

# Install CMake configuration files
install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
)