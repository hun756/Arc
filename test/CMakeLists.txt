# Load Google Test configuration
include(${CMAKE_SOURCE_DIR}/cmake/FindGTest.cmake)  # Custom module to find and configure GTest
include(GoogleTest)  # Built-in CMake module for GTest integration

# Find all test files in the current directory
file(GLOB TEST_SOURCES "*.cpp")  # Collects all C++ files into TEST_SOURCES variable

# Process each test file individually
foreach(test_source ${TEST_SOURCES})
    # Extract the base name without extension to use as the test target name
    get_filename_component(test_name ${test_source} NAME_WE)
    
    # Create an executable for this test
    add_executable(${test_name} ${test_source})
    
    # Link necessary libraries to the test
    target_link_libraries(${test_name}
        PRIVATE           # These dependencies are only used internally by the test
        GTest::gtest      # Google Test framework
        GTest::gtest_main # Google Test main entry point
        ${PROJECT_NAME}::${PROJECT_NAME}  # Link to the main project library
    )
    
    # Apply warning configuration from elsewhere in the project
    target_compile_warnings(${test_name} PRIVATE)

    # Enable sanitizers for this test if requested at configuration time
    if(ENABLE_SANITIZERS)
        target_enable_sanitizers(${test_name})
    endif()
    
    # Enable code coverage instrumentation if requested
    if(ENABLE_COVERAGE)
        target_compile_options(${test_name} PRIVATE --coverage)
        target_link_libraries(${test_name} PRIVATE --coverage)
    endif()
    
    # Register with CTest for test discovery and running
    add_test(NAME ${test_name} COMMAND ${test_name})
    
    # Set a reasonable timeout to prevent tests from hanging indefinitely
    set_tests_properties(${test_name} PROPERTIES TIMEOUT 10)
endforeach()