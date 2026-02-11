file (GLOB INCLUDE_HEADERS "headers/*")
file (GLOB INCLUDE_SOURCES "src/*")
add_library (rendering STATIC ${INCLUDE_HEADERS} ${INCLUDE_SOURCES})

target_include_directories (rendering BEFORE PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/dep/include ${CMAKE_CURRENT_SOURCE_DIR}/headers)

# Copy dlls
if (NOT DEFINED MAIN_BINARY_DIR)
	set (MAIN_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR})
endif ()

file (GLOB DYNAMIC_LIBS "${CMAKE_CURRENT_SOURCE_DIR}/dep/lib/*.dll")
file (COPY ${DYNAMIC_LIBS} DESTINATION ${MAIN_BINARY_DIR})

# Linking sdl
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
	target_link_libraries (rendering PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/dep/lib/SDL3d.lib)
else ()
	target_link_libraries (rendering PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/dep/lib/SDL3.lib)
endif()

# Linking vulkan
target_link_libraries (rendering PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/dep/lib/vulkan-1.lib)


target_link_libraries (rendering PRIVATE ${COMMON_SOURCE_DIR})

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET rendering PROPERTY CXX_STANDARD 20)
endif()
