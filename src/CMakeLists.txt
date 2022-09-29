include_directories("${Paraordenar_SOURCE_DIR}/inc" )
include_directories("${Paraordenar_SOURCE_DIR}/paraordenar-core")
include_directories(${CMAKE_BINARY_DIR}/paraordenar-core)
include_directories(${BOOST_INCLUDE_DIR})
include_directories(${FMT_INCLUDE_DIR})

link_directories(${Paraordenar_SOURCE_DIR}/lib)

add_executable(Paraordenar paraordenar.cc cli_visualizer.cc)
set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "pro")

target_link_libraries(Paraordenar PUBLIC 
                        ParaordenarCore 
                        ${BOOST_REGEX}
                        ${BOOST_FS} 
                        ${BOOST_DT}
                        ${FMT_LIB}
)


install(TARGETS Paraordenar DESTINATION bin)
install(FILES "../inc/cli_visualizer.hpp" "../inc/CLI11.hpp" "../inc/pro_defines.hpp"
  DESTINATION include
  )
