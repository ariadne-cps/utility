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

    add_library(utility ${LIBRARY_KIND} $<TARGET_OBJECTS:UTILITY_SRC>)
    set_target_properties(utility PROPERTIES OUTPUT_NAME "ariadne-utility")
    target_link_libraries(utility)

    setup_standalone_project_tests(
        TARGET utility
        TEST_DIRECTORY test
        TEST_TARGET tests
        SOURCES
            "${PROJECT_SOURCE_DIR}/include"
            "${PROJECT_SOURCE_DIR}/src"
    )

endif()
