# Link Time Optimization allows the compiler to perform optimizations across
# translation units, potentially improving performance and reducing binary size.
# This module enables LTO when supported by your compiler.
if(ENABLE_LTO)
    include(CheckIPOSupported)
    check_ipo_supported(RESULT result OUTPUT output)
    if(result)
        set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
    else()
        message(WARNING "LTO is not supported: ${output}")
    endif()
endif()
