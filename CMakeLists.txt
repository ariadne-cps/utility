cmake_minimum_required(VERSION 3.15)

project(Utility VERSION 1.0)

option(WARNINGS_AS_ERRORS "Treat compiler warnings as errors" OFF)

if(NOT TARGET ariadne-configuration)
    find_package(Git REQUIRED)
    execute_process(COMMAND "${GIT_EXECUTABLE}" -C "${CMAKE_CURRENT_SOURCE_DIR}"
                    submodule update --init -- ss/cfg
                    RESULT_VARIABLE _result)
    if(_result)
        message(FATAL_ERROR "Cannot initialize configuration submodule")
    endif()
    include("${CMAKE_CURRENT_SOURCE_DIR}/ss/cfg/cmake/ProjectOptions.cmake")
endif()



setup_project()

include_directories(${PROJECT_SOURCE_DIR}/include)

add_subdirectory(ss)

if(NOT TARGET utility)
    add_library(utility INTERFACE)
    target_include_directories(utility INTERFACE
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include/ariadne-utility>
    )

    add_subdirectory(src)
    target_link_libraries(UTILITY_SRC PRIVATE utility)
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
    USE_ARIADNE_TESTING
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

# Documentation targets are standalone-only to avoid conflicts in parent builds.
if(CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR)
  include("${CMAKE_CURRENT_SOURCE_DIR}/ss/cfg/cmake/UseDoxygen.cmake")
  ariadne_setup_documentation(
    INPUT "${CMAKE_CURRENT_SOURCE_DIR}/include" "${CMAKE_CURRENT_SOURCE_DIR}/src"
    IMAGE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/doc"
  )
endif()
