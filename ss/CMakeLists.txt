if(NOT TARGET macro)
    ensure_submodule_initialized(macro)
    add_subdirectory(macro)
endif()
