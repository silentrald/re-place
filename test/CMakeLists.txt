
add_compile_definitions(RP_TEST)

# Unit Test
file(GLOB TEST_UNIT_SRC "unit/*.cpp")

add_executable(test_unit ${TEST_UNIT_SRC}
  ${CMAKE_SOURCE_DIR}/src/types.cpp
  ${CMAKE_SOURCE_DIR}/src/utils/logger/silent.cpp
  ${CMAKE_SOURCE_DIR}/src/utils/crypto/password.cpp
)

target_link_libraries(test_unit PRIVATE
  ds
  uuid argon2
)

# Integration Test
file(GLOB TEST_INTEGRATION_SRC "integration/*.cpp")

add_executable(test_integration ${TEST_INTEGRATION_SRC}
  ${CMAKE_SOURCE_DIR}/src/types.cpp
  ${CMAKE_SOURCE_DIR}/src/utils/logger/silent.cpp
  ${CMAKE_SOURCE_DIR}/src/utils/crypto/password.cpp

  ${CMAKE_SOURCE_DIR}/src/entity/user.cpp
)

target_link_libraries(test_integration PRIVATE
  ds
  uuid argon2
)
