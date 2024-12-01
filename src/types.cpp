/*============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2024-11-24
 *============================*/

#include "./types.hpp"

#define REQUIRED(field) field " should not be empty";
#define STRING_INDIRECTION(x) #x
#define MAX_LENGTH(field, max_value)                                           \
  field " is too long (max " STRING_INDIRECTION(max_value) " characters)"
#define MIN_LENGTH(field, min_value)                                           \
  field " is too short (min " STRING_INDIRECTION(min_value) " characters)"

const c8* rp::get_error_string(error_code code) noexcept {
  switch ((error)code) {
  case error::OK:
    return "OK";

  case error::BAD_ALLOCATION:
    return "Bad allocation, no more memory";

  case error::NULL_VALUE:
    return "Accessing null value";

  case error::INDEX_OUT_OF_BOUNDS:
    return "Index out of bounds";

  case error::CONTAINER_EMPTY:
    return "Container is empty";

  case error::CONTAINER_FULL:
    return "Container is full";

  case error::NOT_FOUND:
    return "Not found";

  case error::INVALID_SIZE:
    return "Invalid size";

  case error::NOT_IMPLEMENTED:
    return "Not yet implemented";

  case error::THREAD_ERROR:
    return "Thread error";

  case error::SERVER_INIT_ERROR:
    return "Could not initialize the server";

  case error::SERVER_RUN_ERROR:
    return "Encountered an error while running the server";

  case error::CRYPTO_HASH_ERROR:
    return "Could not hash string";

  // DB
  case error::DB_CONNECTION_ERROR:
    return "Could not connect to the database";

  case error::DB_PREPARE_ERROR:
    return "Could not prepare the query to the database";

  case error::DB_EXECUTION_ERROR:
    return "Could not execute the query to the database";

  case error::VALKEY_CONNECTION_ERROR:
    return "Could not connect to valkey";

  case error::VALKEY_COMMAND_ERROR:
    return "Could not execute command in valkey";

  case error::VALKEY_TYPE_ERROR:
    return "Invalid return type in valkey";

  case error::VALKEY_NONE_DELETED_ERROR:
    return "No record was deleted in valkey";

  // Entities
  case error::USER_USERNAME_REQUIRED:
    return REQUIRED("Username");

  case error::USER_USERNAME_MAX_LENGTH:
    return MAX_LENGTH("Username", User::USERNAME_MAX);

  case error::USER_PASSWORD_REQUIRED:
    return REQUIRED("Password");

  case error::USER_PASSWORD_MIN_LENGTH:
    return MIN_LENGTH("Password", User::PASSWORD_MIN_STRING);

  case error::USER_PASSWORD_MAX_LENGTH:
    return MAX_LENGTH("Password", User::PASSWORD_MAX_STRING);

  // Repositories
  case error::REPO_USER_NOT_FOUND:
    return "User not found";

  // Use cases
  case error::USE_CASE_AUTH_LOGIN_USERNAME_EMPTY:
    return REQUIRED("Username");

  case error::USE_CASE_AUTH_LOGIN_PASSWORD_EMPTY:
    return REQUIRED("Password");

  case error::USE_CASE_AUTH_LOGIN_PASSWORD_MISMATCH:
    return "Password mismatch";

#ifdef RP_TEST
  case error::TEST_NOT_MATCHING_TAGS:
    return nullptr;

  case error::TEST_EXPECT_FAILED:
    return "Expect failed";

  case error::TEST_RETURN_FAIL:
    return "Return failure";
#endif
  }

  return "unknown";
}
