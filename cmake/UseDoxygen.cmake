# - Run Doxygen
#
# Adds a doxygen target that runs doxygen to generate the html
# and optionally the LaTeX API documentation.
# The doxygen target is added to the doc target as a dependency.
# i.e.: the API documentation is built with:
#  make doc
#
# USAGE: GLOBAL INSTALL
#
# Install it with:
#  cmake ./ && sudo make install
# Add the following to the CMakeLists.txt of your project:
#  include(UseDoxygen OPTIONAL)
# Optionally copy Doxyfile.in in the directory of CMakeLists.txt and edit it.
#
# USAGE: INCLUDE IN PROJECT
#
#  set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR})
#  include(UseDoxygen)
# Add the Doxyfile.in and UseDoxygen.cmake files to the projects source directory.
#
#
# CONFIGURATION
#
# To configure Doxygen you can edit Doxyfile.in and set some variables in cmake.
# Variables you may define are:
#  DOXYFILE_SOURCE_DIR - Path where the Doxygen input files are.
#  	Defaults to the current source directory.
#  DOXYFILE_EXTRA_SOURCES - Additional source diretories/files for Doxygen to scan.
#  	The Paths should be in double quotes and separated by space. e.g.:
#  	 "${CMAKE_CURRENT_BINARY_DIR}/foo.c" "${CMAKE_CURRENT_BINARY_DIR}/bar/"
#  
#  DOXYFILE_OUTPUT_DIR - Path where the Doxygen output is stored.
#  	Defaults to "${CMAKE_CURRENT_BINARY_DIR}/doc".
#  
#  DOXYFILE_LATEX - ON/OFF; Set to "ON" if you want the LaTeX documentation
#  	to be built.
#  DOXYFILE_LATEX_DIR - Directory relative to DOXYFILE_OUTPUT_DIR where
#  	the Doxygen LaTeX output is stored. Defaults to "latex".
#  
#  DOXYFILE_HTML_DIR - Directory relative to DOXYFILE_OUTPUT_DIR where
#  	the Doxygen html output is stored. Defaults to "html".
#

#
#  Copyright (c) 2009, 2010, 2011 Tobias Rautenkranz <tobias@rautenkranz.ch>
#
#  Redistribution and use is allowed according to the terms of the New
#  BSD license.
#  For details see the accompanying COPYING-CMAKE-SCRIPTS file.
#


# Adapted for Utility: preserve Ariadne's 'doxygen' and 'doc' targets,
# generating XML for the independent Markdown stage.
find_package(Doxygen QUIET)
if(NOT DOXYGEN_FOUND)
  message(STATUS "Doxygen not found: documentation targets unavailable")
  return()
endif()
set(DOXYFILE_OUTPUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/docs" CACHE PATH "Doxygen output directory")
option(DOXYFILE_LATEX "Generate LaTeX sources" ON)
if(DOXYFILE_LATEX)
  set(DOXYFILE_GENERATE_LATEX YES)
else()
  set(DOXYFILE_GENERATE_LATEX NO)
endif()
set(DOXYFILE "${CMAKE_CURRENT_BINARY_DIR}/Doxyfile")
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/doc/Doxyfile.in" "${DOXYFILE}" @ONLY)
add_custom_target(doxygen
  COMMAND "${DOXYGEN_EXECUTABLE}" "${DOXYFILE}"
  COMMENT "Building Doxygen HTML/XML/LaTeX documentation"
  WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
  VERBATIM)
add_custom_target(doc DEPENDS doxygen)
add_custom_target(doc-html DEPENDS doxygen)

find_package(Python3 COMPONENTS Interpreter QUIET)
if(Python3_Interpreter_FOUND)
  add_custom_target(doc-markdown
    COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/scripts/build_doc_markdown.py"
      --xml "${DOXYFILE_OUTPUT_DIR}/xml" --output "${DOXYFILE_OUTPUT_DIR}/markdown"
    DEPENDS doxygen
    COMMENT "Converting Utility Doxygen XML to Markdown for Hugo"
    VERBATIM)
else()
  message(STATUS "Python3 not found: doc-markdown unavailable")
endif()

if(DOXYFILE_LATEX)
  find_program(DOXYFILE_MAKE make)
  find_program(DOXYFILE_PDFLATEX pdflatex)
  if(DOXYFILE_MAKE AND DOXYFILE_PDFLATEX)
    add_custom_target(doc-pdf COMMAND "${DOXYFILE_MAKE}"
      WORKING_DIRECTORY "${DOXYFILE_OUTPUT_DIR}/latex"
      DEPENDS doxygen
      COMMENT "Building PDF from Doxygen LaTeX" VERBATIM)
  else()
    message(STATUS "make and pdflatex required for doc-pdf")
  endif()
endif()
