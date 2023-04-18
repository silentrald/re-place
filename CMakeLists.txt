cmake_minimum_required(VERSION 2.8.12)
# cmake_policy(SET CMP0048 NEW)

project(clean)

set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

add_executable(${PROJECT_NAME} src/main.cpp
  src/entity/user.cpp

  src/repo/pg/user.cpp

  src/api/asio/connection.cpp
  src/api/asio/connection_manager.cpp
  src/api/asio/mime_types.cpp
  src/api/asio/reply.cpp
  src/api/asio/request_handler.cpp
  src/api/asio/request_parser.cpp
  src/api/asio/server.cpp

  src/api/auth.cpp
)
include_directories(src)

add_subdirectory(external/ds)
include_directories(external/ds/src)
include_directories(external/asio/asio/include)

set(BUILD_SHARED_LIBS OFF)
set(BUILD_STATIC_LIBS ON)
add_subdirectory(external/llhttp)
include_directories(external/llhttp/include)

target_link_libraries(${PROJECT_NAME} PRIVATE
  ds llhttp_static
)

