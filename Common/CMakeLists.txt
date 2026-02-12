set (COMMON_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src PARENT_SCOPE)


add_executable (commonLMTests "tests/lm2_tests.cpp")

target_include_directories (commonLMTests BEFORE PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET commonLMTests PROPERTY CXX_STANDARD 20)
endif()
