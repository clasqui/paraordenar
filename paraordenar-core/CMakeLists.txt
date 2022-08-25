add_library(ParaordenarCore project.cc pugixml.cc storage.cc)

include_directories(${Paraordenar_SOURCE_DIR}/paraordenar-core)
include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${BOOST_INCLUDE_DIR})

install(TARGETS ParaordenarCore DESTINATION ${Paraordenar_SOURCE_DIR}/lib)
install(FILES project.hpp pugiconfig.hpp pugixml.hpp storage.hpp types.h DESTINATION include)