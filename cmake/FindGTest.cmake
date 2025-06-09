include(FetchContent)

# Define where to get Google Test from
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG release-1.12.1
)

# Show download progress (useful for transparency)
set(FETCHCONTENT_QUIET OFF)

# Force Google Test to use the same runtime library as your project
# (Prevents runtime library mismatches on Windows)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

# Actually retrieve and make the dependency available
FetchContent_MakeAvailable(googletest)
