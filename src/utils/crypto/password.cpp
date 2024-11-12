/*============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2024-11-11
 *============================*/

#include "./crypto.hpp"
#include "argon2.h"
#include "types.hpp"
#include "utils/logger/logger.hpp"

inline const u32 ITERATIONS = 2;
inline const u32 MEMORY = 19'456;
inline const u32 PARALLELISM = 1;
// TODO: .env configurable
inline const c8* const SALT = "some-random-salt";
inline const u32 SALT_LENGTH = std::strlen(SALT);
inline const u32 OUTPUT_LENGTH = 32;
inline const u32 ENCODED_LENGTH = 108;

expected<string, error_code> crypto::hash_password(const string& password
) noexcept {
  return std::move(hash_password(password.c_str(), password.get_size()));
}

expected<string, error_code> crypto::hash_password(const c8* password
) noexcept {
  return std::move(hash_password(password, std::strlen(password)));
}

expected<string, error_code>
crypto::hash_password(const c8* password, usize size) noexcept {
  c8 encoded[ENCODED_LENGTH]; // NOLINT
  i32 argon_error = argon2id_hash_encoded(
      ITERATIONS, MEMORY, PARALLELISM, password, size, SALT, SALT_LENGTH,
      OUTPUT_LENGTH, encoded, ENCODED_LENGTH
  );

  if (argon_error == ARGON2_OK) {
    string hash{};
    RP_TRY(hash.copy(encoded), rp::to_unexpected);
    return std::move(hash);
  }

  if (argon_error == ARGON2_MEMORY_ALLOCATION_ERROR) {
    return unexpected<error_code>{error::BAD_ALLOCATION};
  }

  logger::error("Received argon2 error: %s", argon2_error_message(argon_error));
  return unexpected<error_code>{error::CRYPTO_HASH_ERROR};
}

bool crypto::verify_password(
    const string& hash, const string& password
) noexcept {
  return verify_password(hash, password.c_str(), password.get_size());
}

bool crypto::verify_password(const string& hash, const c8* password) noexcept {
  return verify_password(hash, password, strnlen(password, 60));
}

bool crypto::verify_password(
    const string& hash, const c8* password, usize size
) noexcept {
  return argon2id_verify(hash.c_str(), password, size) == ARGON2_OK;
}
