cmake_minimum_required(VERSION 3.12)

# set the project name and version
project(Paraordenar VERSION 0.1)

option(BUILD_SHARED_LIBS "Build using shared libraries" ON)

####### LIBRARIES CONFIG #######

set(BOOST_DIR $ENV{BOOST_DIR} CACHE PATH "Boost installation directory")
set(BOOST_INCLUDE_DIR "${BOOST_DIR}" CACHE INTERNAL "")
set(BOOST_LIB_DIR "${BOOST_DIR}/lib" CACHE INTERNAL "")

set(FMT_DIR $ENV{FMT_DIR} CACHE PATH "FMT installation directory")
set(FMT_INCLUDE_DIR "${FMT_DIR}/include" CACHE INTERNAL "")
set(FMT_LIB_DIR "${FMT_DIR}/lib" CACHE INTERNAL "")

find_library(BOOST_REGEX
    NAMES boost_regex
    PATHS ${BOOST_DIR} ${BOOST_LIB_DIR}
    REQUIRED
)

find_library(BOOST_FS
    NAMES boost_filesystem
    PATHS ${BOOST_DIR} ${BOOST_LIB_DIR}
    REQUIRED
)

find_library(BOOST_DT
    NAMES boost_date_time
    PATHS ${BOOST_DIR} ${BOOST_LIB_DIR}
    REQUIRED
)

find_library(FMT_LIB
    NAMES fmt
    PATHS ${FMT_DIR} ${FMT_LIB_DIR}
    REQUIRED
)

####### BUILD TYPE #########

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    message(STATUS "Posem tipus de compilació per defecte Release")
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build." FORCE)
endif()
  # Set the possible values of build type for cmake-gui
set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")

####### CONFIG FILE ########
configure_file(paraordenar-core/ParaordenarConfig.h.in paraordenar-core/ParaordenarConfig.h)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_subdirectory(paraordenar-core)
add_subdirectory(src)

####### DOCS #######

option(BUILD_DOC "Build documentation" ON)

if(BUILD_DOC)
find_package(Doxygen)
    if (DOXYGEN_FOUND)
        # set input and output files
        set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/docs/Doxyfile.in)
        set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

        # request to configure the file
        configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)
        message("-- Doxygen build started")

        # note the option ALL which allows to build the docs together with the application
        add_custom_target( docs ALL
            COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
            COMMAND cd docs/latex && make
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
            COMMENT "Generating API documentation with Doxygen"
            )
            install(DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/docs/man"
            DESTINATION share/
            )
            install(FILES "${CMAKE_CURRENT_BINARY_DIR}/docs/latex/refman.pdf"
            DESTINATION share/doc/
            )
            install(DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/docs/html"
            DESTINATION share/doc/
            )
        
    else (DOXYGEN_FOUND)
    message("-! Doxygen need to be installed to generate the doxygen documentation")
    endif (DOXYGEN_FOUND)
endif(BUILD_DOC)