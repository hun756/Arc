# =============================================================================
# Sanitizers - Enhanced code quality tools for C/C++ development
# =============================================================================
#
# What are sanitizers?
# -------------------
# Sanitizers are dynamic analysis tools that detect runtime errors such as:
# - Memory leaks and buffer overflows (Address Sanitizer)
# - Use of uninitialized memory (Memory Sanitizer)
# - Data races (Thread Sanitizer)
# - Undefined behavior (Undefined Behavior Sanitizer)
# - Memory leaks specifically (Leak Sanitizer)
#
# Usage:
# ------
# In your CMakeLists.txt:
#   include(cmake/Sanitizers.cmake)
#   target_enable_sanitizers(your_target_name)
#
# Configuration:
# -------------
# Enable/disable specific sanitizers by setting options:
#   cmake -DENABLE_SANITIZER_ADDRESS=ON/OFF
#   cmake -DENABLE_SANITIZER_LEAK=ON/OFF
#   cmake -DENABLE_SANITIZER_UNDEFINED=ON/OFF
#   cmake -DENABLE_SANITIZER_THREAD=ON/OFF
#   cmake -DENABLE_SANITIZER_MEMORY=ON/OFF
#
# Compatibility Notes:
# ------------------
# - Thread Sanitizer is incompatible with Address/Leak Sanitizer
# - Memory Sanitizer is incompatible with Address/Leak/Thread Sanitizer
# - For best results, use one sanitizer at a time in debug builds
#
# Examples:
# --------
# Address Sanitizer (default):
#   cmake -DCMAKE_BUILD_TYPE=Debug ..
#
# Thread Sanitizer:
#   cmake -DCMAKE_BUILD_TYPE=Debug -DENABLE_SANITIZER_ADDRESS=OFF -DENABLE_SANITIZER_THREAD=ON ..
#
# =============================================================================

function(target_enable_sanitizers target)
    if(CMAKE_CXX_COMPILER_ID MATCHES ".*Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        set(SANITIZERS "")

        # Address Sanitizer: Detects memory errors like buffer overflows, 
        # use-after-free, and memory leaks
        option(ENABLE_SANITIZER_ADDRESS "Enable address sanitizer" TRUE)
        if(ENABLE_SANITIZER_ADDRESS)
            list(APPEND SANITIZERS "address")
        endif()

        # Leak Sanitizer: Specifically focuses on memory leak detection
        # Note: ASan already includes leak detection on most platforms
        option(ENABLE_SANITIZER_LEAK "Enable leak sanitizer" FALSE)
        if(ENABLE_SANITIZER_LEAK)
            list(APPEND SANITIZERS "leak")
        endif()

        # Undefined Behavior Sanitizer: Catches undefined behavior like 
        # null dereferences, signed integer overflow, and more
        option(ENABLE_SANITIZER_UNDEFINED "Enable undefined behavior sanitizer" TRUE)
        if(ENABLE_SANITIZER_UNDEFINED)
            list(APPEND SANITIZERS "undefined")
        endif()

        # Thread Sanitizer: Detects data races and other threading issues
        option(ENABLE_SANITIZER_THREAD "Enable thread sanitizer" FALSE)
        if(ENABLE_SANITIZER_THREAD)
            if("address" IN_LIST SANITIZERS OR "leak" IN_LIST SANITIZERS)
                message(WARNING "Thread sanitizer does not work with Address or Leak sanitizer enabled")
            else()
                list(APPEND SANITIZERS "thread")
            endif()
        endif()

        # Memory Sanitizer: Detects use of uninitialized memory
        # Note: Requires that all code (including libraries) be instrumented
        option(ENABLE_SANITIZER_MEMORY "Enable memory sanitizer" FALSE)
        if(ENABLE_SANITIZER_MEMORY)
            if("address" IN_LIST SANITIZERS OR "leak" IN_LIST SANITIZERS OR "thread" IN_LIST SANITIZERS)
                message(WARNING "Memory sanitizer does not work with Address, Leak or Thread sanitizer enabled")
            else()
                list(APPEND SANITIZERS "memory")
            endif()
        endif()

        list(JOIN SANITIZERS "," LIST_OF_SANITIZERS)
    endif()

    if(LIST_OF_SANITIZERS)
        if(NOT "${LIST_OF_SANITIZERS}" STREQUAL "")
            target_compile_options(${target} PRIVATE -fsanitize=${LIST_OF_SANITIZERS})
            target_link_options(${target} PRIVATE -fsanitize=${LIST_OF_SANITIZERS})
        endif()
    endif()
endfunction()