cmake_minimum_required(VERSION 3.22.1)

project(main VERSION 1.0.0)

set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

add_executable(${PROJECT_NAME}
  src/main.cpp
  src/utils/logger/dev.cpp # TODO: Separate the logger to a lib

  src/entity/user.cpp

  src/repo/pg.cpp
  src/repo/user/pg.cpp

  # src/cache/redis.cpp
  # src/cache/redis/session.cpp

  src/api/asio/connection.cpp
  src/api/asio/connection_manager.cpp
  src/api/asio/mime_types.cpp
  src/api/asio/response.cpp
  src/api/asio/request_handler.cpp
  src/api/asio/server.cpp
)
include_directories(src)

# set(DS_THREAD on)
add_subdirectory(external/ds)
include_directories(external/rapidjson/include)
include_directories(external/ds/src)
include_directories(external/asio/asio/include)

add_subdirectory(external/llhttp)
include_directories(external/llhttp/include)

# add_subdirectory(external/hiredis)

target_link_libraries(${PROJECT_NAME} PRIVATE
  ds
  # ds-thread
  pq
  llhttp_shared
  # hiredis
)

