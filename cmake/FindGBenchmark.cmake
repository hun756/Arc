# This module automatically downloads and integrates Google Benchmark into your project.
# It handles configuration, compiler options, and makes the library available to your targets.

include(FetchContent)

# Declare what we want to fetch - Google Benchmark in this case
FetchContent_Declare(
    googlebenchmark
    GIT_REPOSITORY https://github.com/google/benchmark.git
    GIT_TAG v1.7.1
)

# Make CMake show us download progress (normally hidden)
set(FETCHCONTENT_QUIET OFF)

# Control benchmark's behavior through CMake options
# The CACHE BOOL "..." FORCE syntax ensures these settings override any defaults

# Disable benchmark's internal tests - we just want the library
set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "Disable benchmark testing" FORCE)

# Prevent benchmark from installing itself system-wide
set(BENCHMARK_ENABLE_INSTALL OFF CACHE BOOL "Disable benchmark install" FORCE)

# Don't treat warnings as errors - helps with cross-platform compatibility
set(BENCHMARK_ENABLE_WERROR OFF CACHE BOOL "Disable treating warnings as errors" FORCE)

# Actually make the content available to our build system
FetchContent_MakeAvailable(googlebenchmark)

# Only apply these settings if benchmark target exists
if(TARGET benchmark)
  if(MSVC)
    target_compile_options(benchmark PRIVATE /W3 /WX-)
    target_compile_options(benchmark_main PRIVATE /W3 /WX-)
  else()
    target_compile_options(benchmark PRIVATE -Wall -Wextra -Wno-error -Wno-invalid-offsetof -Wno-error=invalid-offsetof)
    target_compile_options(benchmark_main PRIVATE -Wall -Wextra -Wno-error -Wno-invalid-offsetof -Wno-error=invalid-offsetof)
  endif()
endif()