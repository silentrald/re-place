/*============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2024-11-24
 *============================*/

#ifndef RP_TEST_HPP
#define RP_TEST_HPP

#include "types.hpp"
#include <cstdio>
#include <cstring>
#include <type_traits>

#define TEST_INIT(_test_name)                                                  \
  const c8* test_name = _test_name;                                            \
  bool success = true;                                                         \
  error_code code = error::OK;

#define TEST_CASE(test_case_name, tags, test_function)                         \
  if (data.test_tags(tags) == error::OK) {                                     \
    test::Return ret = test_function(test_name, test_case_name);               \
    static_cast<void>(data.returns.push(ret));                                 \
    if (rp::is_error(ret.code)) {                                              \
      success = false;                                                         \
    }                                                                          \
  } else {                                                                     \
    static_cast<void>(data.returns.push(                                       \
        test::Return{.name = test_case_name, .code = error::TEST_SKIPPED}      \
    ));                                                                        \
  }

#define TEST_CASE_FUNCTION(function_name)                                      \
  test::Return function_name(                                                  \
      const c8* test_name, const c8* test_case_name                            \
  ) noexcept

#define TEST_OK return test::Return{.name = test_case_name, .code = error::OK};

#define TEST_END()                                                             \
  printf("=== %s ===\n", test_name);                                           \
  data.finish_test(__FILE__);                                                  \
  return success;

#define TEST_TRY(...) GET_MACRO(__VA_ARGS__, TEST_TRY2, TEST_TRY1)(__VA_ARGS__)

#define TEST_TRY1(expression)                                                  \
  {                                                                            \
    auto code = expression;                                                    \
    if (rp::is_error(code)) {                                                  \
      return test::Return{                                                     \
          .name = test_case_name, .code = std::move(code), .line = __LINE__    \
      };                                                                       \
    }                                                                          \
  }

#define TEST_TRY2(expression, converter)                                       \
  {                                                                            \
    auto code = expression;                                                    \
    if (rp::is_error(code)) {                                                  \
      return test::Return{                                                     \
          .name = test_case_name,                                              \
          .code = std::move(converter(code)),                                  \
          .line = __LINE__                                                     \
      };                                                                       \
    }                                                                          \
  }

// Only used by expected
#define TEST_TRY_RETURN(expression)                                            \
  ({                                                                           \
    auto exp = expression;                                                     \
    if (rp::is_error(exp)) {                                                   \
      return test::Return{                                                     \
          .name = test_case_name, .code = exp.error(), .line = __LINE__        \
      };                                                                       \
    }                                                                          \
    std::move(*exp);                                                           \
  })

#define EXPECT(actual, exp, comparator)                                        \
  if (!test::expect(                                                           \
          test_name, test_case_name, actual, #actual, exp, #exp, comparator    \
      )) {                                                                     \
    return test::Return{                                                       \
        .name = test_case_name,                                                \
        .code = error::TEST_EXPECT_FAILED,                                     \
        .line = __LINE__                                                       \
    };                                                                         \
  }

#define EXPECT_TRUE(function)                                                  \
  {                                                                            \
    if (!function) {                                                           \
      printf(                                                                  \
          "Received Error from: %s > %s\n  %s: false\n", test_name,            \
          test_case_name, #function                                            \
      );                                                                       \
      return test::Return{                                                     \
          .name = test_case_name,                                              \
          .code = error::TEST_EXPECT_FAILED,                                   \
          .line = __LINE__                                                     \
      };                                                                       \
    }                                                                          \
  }

#define TEST_FAIL                                                              \
  return test::Return{                                                         \
      .name = test_case_name,                                                  \
      .code = error::TEST_RETURN_FAIL,                                         \
      .line = __LINE__                                                         \
  };

namespace test {

struct Return {
  const c8* name;
  error_code code;
  u32 line;

  [[nodiscard]] error_code copy(const Return& other) noexcept {
    this->name = other.name;
    this->code = other.code;
    this->line = other.line;
    return error::OK;
  }
};

struct Data {
  vector<Return> returns{};

  const c8* tags = nullptr;
  vector<u32> indexes{};
  vector<u32> memo_indexes{};

  error_code set_tags(const c8* tags) noexcept {
    this->tags = tags;
    usize length = std::strlen(tags) - 1; // subtract ']'
    for (usize i = 1U; i < length; ++i) { // 1U, skip '['
      if (tags[i] == ',') {
        RP_TRY(this->indexes.push(i));
      }
    }
    RP_TRY(this->indexes.push(length));
    return error::OK;
  }

  error_code test_tags(const c8* tags) noexcept {
    if (this->tags == nullptr) {
      return error::OK;
    }

    assert(tags != nullptr);

    this->memo_indexes.clear();

    usize length = std::strlen(tags) - 1; // subtract ']'
    for (usize i = 1U; i < length; ++i) { // 1U, skip '['
      if (tags[i] == ',') {
        RP_TRY(this->memo_indexes.push(i));
      }
    }
    RP_TRY(this->memo_indexes.push(length));

    usize user_start_index = 1U;
    usize user_size = 0U;
    usize start_index = 0U;
    bool found = false;
    for (auto user_end_index : this->indexes) {
      user_size = user_end_index - user_start_index;
      start_index = 1U;
      for (auto end_index : this->memo_indexes) {
        if (user_size == end_index - start_index &&
            std::strncmp(
                this->tags + user_start_index, tags + start_index, user_size
            ) == 0) {
          found = true;
          break;
        }

        start_index = end_index + 1;
      }

      if (!found) {
        return error::TEST_NOT_MATCHING_TAGS;
      }

      found = false;
      user_start_index = user_end_index + 1;
    }

    return error::OK;
  }

  void finish_test(const c8* filename) noexcept {
    for (const auto& ret : this->returns) {
      switch (ret.code) {
      case error::OK:
        printf("\x1B[32m> [OK]   %s\x1B[0m\n", ret.name);
        break;

      case error::TEST_SKIPPED:
        printf("\x1B[34m> [SKIP] %s\x1B[0m\n", ret.name);
        break;

      default:
        printf(
            "\x1B[31m> [BAD]  %s: %s [%s:%u]\x1B[0m\n", ret.name,
            rp::get_error_string(ret.code), filename, ret.line
        );
        break;
      }
    }
    this->returns.clear();
  }
};

enum class ExpectComparator {
  EQUAL,
};

template <typename T, bool IsActual>
void expect_print_error(const T& value) noexcept {
  const c8* label = nullptr;
  if constexpr (IsActual) {
    label = "  Actual";
  } else {
    label = "Expected";
  }

  if constexpr (std::is_same<string, T>::value) {
    printf("  %s: %s\n", label, value.c_str());
  } else if constexpr (std::is_same<const c8*, T>::value) {
    printf("  %s: %s\n", label, value);
  }
}

template <typename ActualType, typename ExpectedType>
[[nodiscard]] bool expect(
    const c8* test_name, const c8* test_case_name, const ActualType& actual,
    const c8* actual_string, const ExpectedType& exp, const c8* exp_string,
    ExpectComparator comparator
) noexcept {
  const c8* comparator_string = nullptr;
  bool success = false;

  switch (comparator) {
  case ExpectComparator::EQUAL:
    comparator_string = "==";
    if constexpr (std::is_same<const c8*, ActualType>::value &&
                  std::is_same<const c8*, ExpectedType>::value) {
      success = std::strcmp(actual, exp) == 0;
    } else {
      success = actual == exp;
    }
    break;
  }

  if (!success) {
    printf("Received Error from: %s > %s\n", test_name, test_case_name);
    expect_print_error<ExpectedType, false>(exp);
    expect_print_error<ActualType, true>(actual);
    printf(
        "Expression: %s %s %s\n", actual_string, comparator_string, exp_string
    );
    return false;
  }

  return true;
}

} // namespace test

#endif
