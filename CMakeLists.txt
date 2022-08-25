cmake_minimum_required(VERSION 3.12)

# set the project name and version
project(Paraordenar VERSION 0.1)

option(BUILD_SHARED_LIBS "Build using shared libraries" ON)

set(BOOST_DIR $ENV{BOOST_DIR} CACHE PATH "Boost installation directory")
set(BOOST_INCLUDE_DIR "${BOOST_DIR}" CACHE INTERNAL "")
set(BOOST_LIB_DIR "${BOOST_DIR}/lib" CACHE INTERNAL "")

set(FMT_DIR $ENV{FMT_DIR} CACHE PATH "FMT installation directory")
set(FMT_INCLUDE_DIR "${FMT_DIR}/inc" CACHE INTERNAL "")
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

configure_file(paraordenar-core/ParaordenarConfig.h.in paraordenar-core/ParaordenarConfig.h)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_subdirectory(paraordenar-core)
add_subdirectory(src)
