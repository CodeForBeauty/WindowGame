add_executable (Main "main.cpp")

target_include_directories (Main BEFORE PRIVATE ${COMMON_INCLUDE_DIR})

target_link_libraries (Main rendering)
target_link_libraries (Main assetLib)

# TEMPORARY
add_custom_target(copy_assets 
					COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/assets" "${MAIN_BINARY_DIR}/assets"
					BYPRODUCTS "${MAIN_BINARY_DIR}/assets"
					COMMENT "Copying assets from ${CMAKE_CURRENT_SOURCE_DIR}/assets into ${MAIN_BINARY_DIR}/assets."
)


add_dependencies (Main copy_assets)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET Main PROPERTY CXX_STANDARD 20)
endif()
