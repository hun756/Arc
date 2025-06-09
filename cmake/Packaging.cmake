#==============================================================================
# PROJECT PACKAGING CONFIGURATION
#==============================================================================
# This file configures CPack, CMake's packaging system, which helps create
# installers and distribution packages for different platforms.
# Learn more: https://cmake.org/cmake/help/latest/module/CPack.html

#------------------------------------------------------------------------------
# Basic Package Information
#------------------------------------------------------------------------------
# These settings define metadata included in all package types
set(CPACK_PACKAGE_VENDOR "Your Organization")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "${PROJECT_DESCRIPTION}")

# Version information is typically extracted from your main CMakeLists.txt
# These values are used to create version-specific package names
set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})

# Include important project files in the package
# LICENSE file is important for compliance and legal requirements
# README provides users with basic information about the package
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
set(CPACK_RESOURCE_FILE_README "${CMAKE_CURRENT_SOURCE_DIR}/README.md")

#------------------------------------------------------------------------------
# Package Generators
#------------------------------------------------------------------------------
# Source package formats - these create archives of the source code
set(CPACK_SOURCE_GENERATOR "TGZ;ZIP")

# Default binary package formats available on all platforms
# TGZ = .tar.gz file (common on Unix)
# ZIP = .zip file (common on Windows)
set(CPACK_GENERATOR "TGZ;ZIP")

# Maintainer information for Debian packages
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Your Name <your.email@example.com>")

#------------------------------------------------------------------------------
# Platform-Specific Packaging
#------------------------------------------------------------------------------
# Linux-specific packaging (excluding macOS)
if(UNIX AND NOT APPLE)
    # DEB = Debian package format (.deb) for Debian, Ubuntu, etc.
    # To add RPM support, add: set(CPACK_GENERATOR ${CPACK_GENERATOR} RPM)
    set(CPACK_GENERATOR ${CPACK_GENERATOR} DEB)

    # Optional: Add Debian-specific settings
    # set(CPACK_DEBIAN_PACKAGE_DEPENDS "libc6 (>= 2.3.1-6), libgcc1 (>= 1:3.4.2-12)")
    # set(CPACK_DEBIAN_PACKAGE_SECTION "devel")
endif()

# Windows-specific packaging
if(WIN32)
    set(CPACK_GENERATOR ${CPACK_GENERATOR} NSIS)
    set(CPACK_NSIS_ENABLE_UNINSTALL_BEFORE_INSTALL ON)
    set(CPACK_NSIS_MODIFY_PATH ON)
endif()

# macOS-specific packaging
if(APPLE)
    set(CPACK_GENERATOR ${CPACK_GENERATOR} productbuild)

    # Optional: To create a DMG file instead/additionally:
    # set(CPACK_GENERATOR ${CPACK_GENERATOR} DragNDrop)

    # Optional: Set bundle properties if your project builds an application
    # set(MACOSX_BUNDLE_ICON_FILE icon.icns)
    # set(MACOSX_BUNDLE_BUNDLE_NAME "${PROJECT_NAME}")
endif()

#------------------------------------------------------------------------------
# Include CPack module to enable packaging targets
#------------------------------------------------------------------------------
# This actually makes the packaging functionality available
# After adding this, you can use these commands:
#   - cmake --build . --target package      # Build binary packages
#   - cmake --build . --target package_source   # Build source packages
include(CPack)
