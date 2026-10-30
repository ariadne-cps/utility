include(CTest)
if(NOT TARGET TESTING_SRC)
    ariadne_add_submodule(testing)
endif()
if(NOT TARGET TESTING_SRC OR NOT TARGET testing)
    message(FATAL_ERROR "Testing submodule did not create its object/interface targets")
endif()

# setup_project_library intentionally does not create ariadne-testing when nested.
# Build its object files into a test-only static archive, never link production Utility.
add_library(utility_testing_runtime STATIC $<TARGET_OBJECTS:TESTING_SRC>)
target_link_libraries(utility_testing_runtime PUBLIC testing)

set(UNIT_TESTS
    test_array
    test_command_line_interface
    test_container
    test_lazy
    test_lru_cache
    test_stack_trace
    test_randomiser
    test_stopwatch
    test_typedefs
)

foreach(TEST ${UNIT_TESTS})
    add_executable(${TEST} ${TEST}.cpp)
    target_link_libraries(${TEST} PRIVATE ariadne-utility utility utility_testing_runtime)
    add_test(NAME ${TEST} COMMAND $<TARGET_FILE:${TEST}>)
    if(WIN32)
        set_tests_properties(${TEST} PROPERTIES
            ENVIRONMENT_MODIFICATION
                "PATH=path_list_prepend:$<TARGET_FILE_DIR:ariadne-utility>"
        )
    endif()
endforeach()

add_custom_target(tests)
add_dependencies(tests ${UNIT_TESTS})
