cmake_minimum_required(VERSION 3.15)

project(Utility VERSION 1.0)

option(WARNINGS_AS_ERRORS "Treat compiler warnings as errors" OFF)

if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/submodules/configuration/cmake/ProjectOptions.cmake")
    message(FATAL_ERROR
        "Git submodules are not initialized. "
        "Run 'git submodule update --init --recursive' before configuring the project.")
endif()

include("${CMAKE_CURRENT_SOURCE_DIR}/submodules/configuration/cmake/ProjectOptions.cmake")

require_initialized_submodules()

setup_project()

include_directories(SYSTEM ${PROJECT_SOURCE_DIR}/include)

if(NOT TARGET utility)
    add_subdirectory(src)

    add_library(utility INTERFACE)
    target_include_directories(utility INTERFACE
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include/ariadne-utility>
    )
endif()

ariadne_register_public_headers(
    TARGET utility
    DIRECTORIES "${PROJECT_SOURCE_DIR}/include/utility"
)

setup_project_library(
    TARGET ariadne-utility
    OBJECTS UTILITY_SRC
    LINK_LIBRARIES utility
)

setup_standalone_project_tests(
    TARGET ariadne-utility
    TEST_DIRECTORY tests
    TEST_TARGET tests
    SOURCES
        "${PROJECT_SOURCE_DIR}/include"
        "${PROJECT_SOURCE_DIR}/src"
)

if(CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR)
    ariadne_install_dependency_bundle(
        TARGET utility
        DESTINATION include/ariadne-utility
    )
endif()
