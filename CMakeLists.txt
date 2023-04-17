cmake_minimum_required(VERSION 2.8.12)
cmake_policy(SET CMP0048 NEW)

project(enroll VERSION 1.0.0)

set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

add_executable(${PROJECT_NAME} src/main.cpp
  src/entity/user.cpp
)
include_directories(src)

add_subdirectory(external/ds)
include_directories(external/ds/src)

set(BUILD_SHARED_LIBS OFF)
set(BUILD_STATIC_LIBS ON)
add_subdirectory(external/llhttp)
include_directories(external/llhttp/include)

target_link_libraries(${PROJECT_NAME} PRIVATE
  ds llhttp_static
)

