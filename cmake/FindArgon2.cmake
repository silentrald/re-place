set(ARGON2_SRCS
  "${CMAKE_SOURCE_DIR}/external/argon2/src/blake2/blake2b.c"
  "${CMAKE_SOURCE_DIR}/external/argon2/src/argon2.c"
  "${CMAKE_SOURCE_DIR}/external/argon2/src/core.c"
  "${CMAKE_SOURCE_DIR}/external/argon2/src/encoding.c"
  "${CMAKE_SOURCE_DIR}/external/argon2/src/thread.c"
  "${CMAKE_SOURCE_DIR}/external/argon2/src/opt.c"
)

include_directories(../external/argon2/include)
add_library(argon2 ${ARGON2_SRCS})

