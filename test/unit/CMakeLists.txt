
add_compile_definitions(RP_TEST)

file(GLOB TEST_UNIT_SRC "*.cpp")

add_executable(test_unit ${TEST_UNIT_SRC}
  ${CMAKE_SOURCE_DIR}/src/types.cpp
  ${CMAKE_SOURCE_DIR}/src/utils/logger/silent.cpp
  ${CMAKE_SOURCE_DIR}/src/utils/crypto/password.cpp
)

target_link_libraries(test_unit PRIVATE
  ds
  uuid argon2
)

