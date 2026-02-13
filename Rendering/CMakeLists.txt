file (GLOB INCLUDE_HEADERS "headers/*")
file (GLOB INCLUDE_SOURCES "src/*")
add_library (rendering STATIC ${INCLUDE_HEADERS} ${INCLUDE_SOURCES})

target_include_directories (rendering BEFORE PUBLIC
		${CMAKE_CURRENT_SOURCE_DIR}/dep/include
		${CMAKE_CURRENT_SOURCE_DIR}/headers
		${COMMON_INCLUDE_DIR}
)

# Copy dlls
if (NOT DEFINED MAIN_BINARY_DIR)
	set (MAIN_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR})
endif ()

file (GLOB DYNAMIC_LIBS "${CMAKE_CURRENT_SOURCE_DIR}/dep/lib/*.dll")
file (COPY ${DYNAMIC_LIBS} DESTINATION ${MAIN_BINARY_DIR})


# Compiling shaders
set (SLANG_COMPILER "${CMAKE_CURRENT_SOURCE_DIR}/slang/slangc.exe")

set (SHADER_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/shaders")
set (SHADER_BINARY_DIR "${MAIN_BINARY_DIR}/shaders")


add_custom_command (
          OUTPUT ${SHADER_BINARY_DIR}
          COMMAND ${CMAKE_COMMAND} -E make_directory ${SHADER_BINARY_DIR}
          COMMENT "Creating ${SHADER_BINARY_DIR}"
)

file (GLOB SHADER_SOURCES "${SHADER_SOURCE_DIR}/*.slang")

foreach (SHADER IN LISTS SHADER_SOURCES)
    get_filename_component (FILE_NAME ${SHADER} NAME_WLE )
    set (SHADER_OUTPUT_FILE ${SHADER_BINARY_DIR}/${FILE_NAME}.spv)
    add_custom_command (
        OUTPUT ${SHADER_OUTPUT_FILE}
        COMMAND ${SLANG_COMPILER} ${SHADER} -target spirv -profile spirv_1_4 -emit-spirv-directly -fvk-use-entrypoint-name -entry vertMain -entry fragMain -o ${SHADER_OUTPUT_FILE}
        DEPENDS ${SHADER}
        COMMENT "Compiling ${SHADER}"
    )

    list (APPEND COMPILED_SHADERS ${SHADER_OUTPUT_FILE})
endforeach()

add_custom_target (compileShaders DEPENDS ${COMPILED_SHADERS} ${SHADER_BINARY_DIR})
add_dependencies (rendering compileShaders)

# Linking sdl
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
	target_link_libraries (rendering PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/dep/lib/SDL3d.lib)
else ()
	target_link_libraries (rendering PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/dep/lib/SDL3.lib)
endif()

# Linking vulkan
target_link_libraries (rendering PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/dep/lib/vulkan-1.lib)


if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET rendering PROPERTY CXX_STANDARD 20)
endif()
