
add_compile_definitions(RP_TEST)

file(GLOB TEST_INTEGRATION_SRC "*.cpp")

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

