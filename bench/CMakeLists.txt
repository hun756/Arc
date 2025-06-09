# Benchmark Configuration
#
# This CMakeLists.txt configures and builds all benchmark executables located in the bench directory.
# 
# The script:
# 1. Includes the FindGBenchmark.cmake module to locate the Google Benchmark library
# 2. Automatically detects all .cpp files in the current directory as benchmark sources
# 3. For each benchmark source file:
#    - Creates an executable with the same name as the source file (without extension)
#    - Links the executable against the main project library and Google Benchmark
#    - Applies optimization flags based on the compiler being used:
#      * -O3 for Clang and GCC compilers
#      * /O2 for MSVC
#
# This approach allows developers to easily add new benchmarks by simply creating
# new .cpp files in the bench directory without modifying this CMakeLists.txt file.
include(${CMAKE_SOURCE_DIR}/cmake/FindGBenchmark.cmake)

file(GLOB BENCH_SOURCES "*.cpp")

foreach(bench_source ${BENCH_SOURCES})
    get_filename_component(bench_name ${bench_source} NAME_WE)
    add_executable(${bench_name} ${bench_source})
    target_link_libraries(${bench_name}
        PRIVATE
        ${PROJECT_NAME}
        benchmark::benchmark
    )

    if(CMAKE_CXX_COMPILER_ID MATCHES ".*Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        target_compile_options(${bench_name} PRIVATE -O3)
    elseif(MSVC)
        target_compile_options(${bench_name} PRIVATE /O2)
    endif()
endforeach()
