cmake_minimum_required(VERSION 3.15)

project(AriadneUtility VERSION 3.0)

option(WARNINGS_AS_ERRORS "Treat compiler warnings as errors" OFF)

if(NOT TARGET ariadne-build)
    find_package(Git REQUIRED)
    # Keep the initial Build checkout messages in English.
    execute_process(
        COMMAND "${CMAKE_COMMAND}" -E env "LC_ALL=C"
                "${GIT_EXECUTABLE}" -C "${CMAKE_CURRENT_SOURCE_DIR}"
                submodule update --init -- ss/bui
        RESULT_VARIABLE _result)
    if(_result)
        message(FATAL_ERROR "Cannot initialize build submodule")
    endif()
    include("${CMAKE_CURRENT_SOURCE_DIR}/ss/bui/cmake/ProjectOptions.cmake")
endif()

require_same_dependency_commit(Build CHAINS
    "build"
    "testing|build")

ariadne_setup_module(utility)

# Documentation remains standalone-only.
if(CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR)
    include("${CMAKE_CURRENT_SOURCE_DIR}/ss/bui/cmake/UseDoxygen.cmake")
    ariadne_setup_documentation(
        INPUT "${CMAKE_CURRENT_SOURCE_DIR}/include"
              "${CMAKE_CURRENT_SOURCE_DIR}/src"
        IMAGE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/doc"
    )
endif()
