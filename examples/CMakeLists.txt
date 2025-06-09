#==============================================================================
# Example Applications
#==============================================================================
# This file builds all the example applications that demonstrate
# how to use your library. Each example is built as a standalone
# executable that links against your main library.

# The GLOB command finds all .cpp files in the current directory
# Note: For production projects, it's generally better to list files
# explicitly for more predictable builds, but for examples, GLOB
# can be convenient as you add new examples without updating CMake.
file(GLOB EXAMPLE_SOURCES "*.cpp")

# For each example source file we found:
foreach(example_source ${EXAMPLE_SOURCES})
    # Extract just the filename without extension (e.g., "basic_usage" from "basic_usage.cpp")
    # This becomes our executable name
    get_filename_component(example_name ${example_source} NAME_WE)

    # Create an executable target for this example
    add_executable(${example_name} ${example_source})

    # Link the example to our main library
    # not for anything that might link to this example (which is unlikely anyway)
    # Using the :: syntax accesses the library through its ALIAS target
    target_link_libraries(${example_name}
        PRIVATE
        ${PROJECT_NAME}::${PROJECT_NAME}
    )

    # Apply the same compiler warnings to our examples as we do to the main library
    # This ensures consistent code quality across the project
    target_compile_warnings(${example_name} PRIVATE)

    # If sanitizers are enabled in the main build, also enable them for examples
    # This helps catch memory/undefined behavior issues in example code too
    if(ENABLE_SANITIZERS)
        target_enable_sanitizers(${example_name})
    endif()

    # You could add example-specific settings here, such as:
    # - Custom compiler definitions
    # - Example-specific include directories
    # - Additional dependencies only needed by certain examples
endforeach()

# Optional: Set example-specific properties or configurations
# set_target_properties(specific_example PROPERTIES ...)