set(LIBRARY_NAME UTILITY_SRC)

add_library(${LIBRARY_NAME} OBJECT
        command_line_interface.cpp
        stack_trace.cpp
        )
