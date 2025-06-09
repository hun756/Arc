#------------------------------------------------------------------------------
# SOURCE FILES
#------------------------------------------------------------------------------
# List all implementation files that make up your application.
# Organization tips:
# - Group related files together
# - Use variable names that clearly describe what the files represent
# - For larger projects, consider using multiple source groups
set(SOURCES
    # Core modules
    ModuleA.cpp  # Handles feature X
    ModuleB.cpp  # Handles feature Y
    
    # Application entry point
    main.cpp     # Program start and orchestration
)

# You can also organize by feature or component
# set(NETWORKING_SOURCES
#     NetworkClient.cpp
#     NetworkServer.cpp
# )

#------------------------------------------------------------------------------
# TARGET DEFINITION
#------------------------------------------------------------------------------
# Create the executable from our source files
# ${PROJECT_NAME} comes from the parent CMakeLists.txt
add_executable(${PROJECT_NAME} ${SOURCES})