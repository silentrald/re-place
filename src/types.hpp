/*============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-04-13
 *============================*/

#ifndef REPLACE_TYPES_HPP
#define REPLACE_TYPES_HPP

#include "ds/bptree_map.hpp"
#include "ds/compare.hpp"
#include "ds/equal.hpp"
#include "ds/expected.hpp"
#include "ds/hash.hpp"
#include "ds/hash_map.hpp"
#include "ds/optional.hpp"
#include "ds/shared_ptr.hpp"
#include "ds/string.hpp"
#include "ds/unique_ptr.hpp"
#include "ds/vector.hpp"
#include <array>
#include <cassert>
#include <cstdint>
#include <cstring>
#include <functional>
#include <set>
#include <uuid/uuid.h>

// === Primitives === //

using i8 = int8_t;
using i16 = short;
using i32 = int;
using i64 = long long;

using u8 = uint8_t;
using u16 = unsigned short;
using u32 = unsigned int;
using u64 = unsigned long long;

using isize = i64;
using usize = u64;

using f32 = float;
using f64 = double;

using c8 = char;

// === DS === //

using string = ds::string;
template <typename T, i32 Size> using array = std::array<T, Size>;
template <typename T> using vector = ds::vector<T>;
template <
    typename Key, typename Value, typename Hash = ds::hash<Key>,
    typename Equal = ds::equal<Key>>
using hash_map = ds::hash_map<Key, Value, Hash, Equal>;
template <typename Key, typename Value, typename KeyCompare = ds::compare<Key>>
using map = ds::bptree_map<Key, Value, KeyCompare>;
template <typename T> using unique_ptr = ds::unique_ptr<T>;
template <typename T> using shared_ptr = ds::shared_ptr<T>;
template <typename T> using set = std::set<T>;

const auto null = ds::null;

template <typename Signature> using function = std::function<Signature>;

// Threading
// using mutex = ds::mutex;

// Error Handling
using error_code = u32;
template <typename T, typename E> using expected = ds::expected<T, E>;
template <typename E> using unexpected = ds::unexpected<E>;

enum error : u32 {
  // COPIED from ds/types.hpp
  OK = 0,
  BAD_ALLOCATION = 1,
  NULL_VALUE = 2, // Accessing null pointer
  INDEX_OUT_OF_BOUNDS = 3,
  CONTAINER_EMPTY = 4,
  CONTAINER_FULL = 5,
  NOT_FOUND = 6,
  INVALID_SIZE = 7, // When capacity is set to a negative value
  NOT_IMPLEMENTED = 8,
  THREAD_ERROR = 9,

  SERVER_INIT_ERROR = 10,
  SERVER_RUN_ERROR,

  CRYPTO_HASH_ERROR,

  // DB
  DB_CONNECTION_ERROR,
  DB_PREPARE_ERROR,
  DB_EXECUTION_ERROR,

  VALKEY_CONNECTION_ERROR,
  VALKEY_COMMAND_ERROR,
  VALKEY_TYPE_ERROR,
  VALKEY_NONE_DELETED_ERROR,

  // Entities
  USER_USERNAME_REQUIRED,
  USER_USERNAME_MAX_LENGTH,
  USER_PASSWORD_REQUIRED,
  USER_PASSWORD_MIN_LENGTH,
  USER_PASSWORD_MAX_LENGTH,

  // Repositories
  REPO_USER_NOT_FOUND,

  // Use cases
  USE_CASE_AUTH_LOGIN_USERNAME_EMPTY,
  USE_CASE_AUTH_LOGIN_PASSWORD_EMPTY,
  USE_CASE_AUTH_LOGIN_PASSWORD_MISMATCH,

#ifdef RP_TEST
  // Test Cases
  TEST_NOT_MATCHING_TAGS,
  TEST_SKIPPED,
  TEST_EXPECT_FAILED,
  TEST_RETURN_FAIL,
#endif
};

namespace rp {

const c8* get_error_string(error_code code) noexcept;

inline bool is_error(error_code code) noexcept {
  return code != error::OK;
}

template <typename T>
inline bool is_error(const expected<T, error_code>& exp) noexcept {
  return !(bool)exp;
}

template <typename T>
inline error_code to_error_code(expected<T, error_code>& exp) noexcept {
  return exp.error();
}

// inline error_code to_error_code(unexpected<error_code>& unexp) noexcept {
//   return unexp.error();
// }

inline unexpected<error_code> to_unexpected(error_code code) noexcept {
  return unexpected<error_code>{code};
}

template <typename T>
inline unexpected<error_code> to_unexpected(expected<T, error_code>& exp
) noexcept {
  return unexpected<error_code>{exp.error()};
}

} // namespace rp

// 00000000-0000-0000-0000-000000000000
class uuid {
public:
  static const u32 CAPACITY = 37U;
  static const u32 SIZE = CAPACITY - 1U;

  uuid() noexcept {
    this->str[0] = '\0';
  };

  explicit uuid(const string& str) noexcept {
    assert(str.get_size() == SIZE);
    std::strncpy(this->str, str.c_str(), SIZE);
    this->str[SIZE] = '\0';
  }

  explicit uuid(const c8* str) noexcept {
    std::strncpy(this->str, str, SIZE);
    this->str[SIZE] = '\0';
  }

  uuid(const uuid& other) noexcept {
    std::strncpy(this->str, other.str, SIZE);
  }

  uuid& operator=(const uuid& rhs) noexcept {
    if (this != &rhs) {
      std::strncpy(this->str, rhs.str, SIZE);
      this->str[SIZE] = '\0';
    }
    return *this;
  }

  uuid(uuid&& other) noexcept {
    std::strncpy(this->str, other.str, SIZE);
  }

  uuid& operator=(uuid&& other) noexcept {
    if (this != &other) {
      std::strncpy(this->str, other.str, SIZE);
    }
    return *this;
  }

  uuid& operator=(const c8* str) noexcept {
    std::strncpy(this->str, str, SIZE);
    this->str[SIZE] = '\0';
    return *this;
  }

  uuid& operator=(const string& str) noexcept {
    assert(str.get_size() == SIZE);
    std::strncpy(this->str, str.c_str(), SIZE);
    this->str[SIZE] = '\0';
    return *this;
  }

  ~uuid() noexcept = default;

  void generate_uuidv4() noexcept {
    uuid_t uuid{};
    uuid_generate_random(uuid);
    uuid_unparse_lower(uuid, this->str);
  }

  [[nodiscard]] const c8* get_string() const noexcept {
    return this->str;
  }

  [[nodiscard]] bool is_empty() const noexcept {
    return this->str[0] == '\0';
  }

  [[nodiscard]] bool operator==(const uuid& rhs) const noexcept {
    return std::strncmp(this->str, rhs.str, SIZE) == 0;
  }

  [[nodiscard]] bool operator==(const c8* str) const noexcept {
    return str[SIZE] != '\0' && std::strncmp(this->str, str, SIZE) == 0;
  }

  [[nodiscard]] bool operator==(const string& str) const noexcept {
    return str.get_size() == SIZE &&
           std::strncmp(this->str, str.c_str(), SIZE) == 0;
  }

  [[nodiscard]] bool operator!=(const uuid& rhs) const noexcept {
    return std::strncmp(this->str, rhs.str, SIZE) != 0;
  }

  [[nodiscard]] bool operator!=(const c8* str) const noexcept {
    return str[SIZE] == '\0' || std::strncmp(this->str, str, SIZE) != 0;
  }

  [[nodiscard]] bool operator!=(const string& str) const noexcept {
    return str.get_size() < SIZE ||
           std::strncmp(this->str, str.c_str(), SIZE) != 0;
  }

  friend bool operator==(const c8* lhs, const uuid& rhs) noexcept {
    return lhs[SIZE] != '\0' && std::strncmp(lhs, rhs.str, SIZE) == 0;
  }

  friend bool operator==(const string& lhs, const uuid& rhs) noexcept {
    return lhs.get_size() == SIZE &&
           std::strncmp(lhs.c_str(), rhs.str, SIZE) == 0;
  }

  friend bool operator!=(const c8* lhs, const uuid& rhs) noexcept {
    return lhs[SIZE] == '\0' || std::strncmp(lhs, rhs.str, SIZE) != 0;
  }

  friend bool operator!=(const string& lhs, const uuid& rhs) noexcept {
    return lhs.get_size() < SIZE ||
           std::strncmp(lhs.c_str(), rhs.str, SIZE) != 0;
  }

private:
  c8 str[40] = {}; // NOLINT
};

// === Macros === //

// NOLINTNEXTLINE
#define RP_GET_MACRO(_1, _2, NAME, ...) NAME

// NOLINTNEXTLINE
#define RP_TRY(...) GET_MACRO(__VA_ARGS__, RP_TRY2, RP_TRY1)(__VA_ARGS__)

// NOLINTNEXTLINE
#define RP_TRY1(expression)                                                    \
  {                                                                            \
    auto error = expression;                                                   \
    if (rp::is_error(error)) {                                                 \
      return std::move(error);                                                 \
    }                                                                          \
  }

// NOLINTNEXTLINE
#define RP_TRY2(expression, converter)                                         \
  {                                                                            \
    auto error = expression;                                                   \
    if (rp::is_error(error)) {                                                 \
      return std::move(converter(error));                                      \
    }                                                                          \
  }

// Only support expected values
// NOLINTNEXTLINE
#define RP_TRY_RETURN(...)                                                     \
  RP_GET_MACRO(__VA_ARGS__, RP_TRY_RETURN2, RP_TRY_RETURN1)(__VA_ARGS__)

// Only used by expected
// NOLINTNEXTLINE
#define RP_TRY_RETURN1(expression)                                             \
  ({                                                                           \
    auto exp = expression;                                                     \
    if (rp::is_error(exp)) {                                                   \
      return std::move(exp);                                                   \
    }                                                                          \
    std::move(*exp);                                                           \
  })

// Only used by expected
// NOLINTNEXTLINE
#define RP_TRY_RETURN2(expression, converter)                                  \
  ({                                                                           \
    auto exp = expression;                                                     \
    if (rp::is_error(exp)) {                                                   \
      return std::move(converter(exp));                                        \
    }                                                                          \
    std::move(*exp);                                                           \
  })

#endif
