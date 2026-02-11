file (GLOB INCLUDE_SOURCES "src/*")
add_library (common STATIC ${INCLUDE_SOURCES})

target_include_directories (common BEFORE PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

set_target_properties(common PROPERTIES LINKER_LANGUAGE CXX)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET common PROPERTY CXX_STANDARD 20)
endif()
