add_library(ParaordenarCore project.cc pugixml.cc storage.cc vault.cc experiment.cc trace.cc)

include_directories(${Paraordenar_SOURCE_DIR}/paraordenar-core)
include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${BOOST_INCLUDE_DIR})

install(TARGETS ParaordenarCore
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib/static
        RUNTIME DESTINATION bin)
install(FILES project.hpp pugiconfig.hpp pugixml.hpp storage.hpp vault.hpp experiment.hpp trace.hpp types.h DESTINATION include)
