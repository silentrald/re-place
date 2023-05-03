/*============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-04-13
 *============================*/

#ifndef CONFIG_TYPES_HPP
#define CONFIG_TYPES_HPP

#include "ds-error/expected.hpp"
#include "ds-error/types.hpp"
#include "ds-thread/mutex.hpp"
#include "ds/optional.hpp"
#include "ds/shared_ptr.hpp"
#include "ds/string.hpp"
#include "ds/types.hpp"
#include "ds/unique_ptr.hpp"
#include "ds/vector.hpp"
#include <cstdint>
#include <functional>
#include <set>

// === Primitives === //
using i8 = int8_t;
using i16 = short;
using i32 = int;
using i64 = long long;

using u8 = uint8_t;
using u16 = unsigned short;
using u32 = unsigned int;
using u64 = unsigned long long;

using f32 = float;
using f64 = double;

// === DS === //
using string = ds::string;
// TODO:
template <typename T, i32 Size> using array = std::array<T, Size>;
template <typename T> using vector = ds::vector<T>;
template <typename T> using unique_ptr = ds::unique_ptr<T>;
template <typename T> using shared_ptr = ds::shared_ptr<T>;
template <typename T> using set = std::set<T>;

const auto null = ds::null;

template <typename Signature> using function = std::function<Signature>;

// Threading
using mutex = ds::mutex;

// Error Handling
using error = ds::error;
using opt_err = ds::opt_err;
template <typename T> using exp_err = ds::exp_err<T>;
using unexp_err = ds::unexpected<error>;

// Error Codes
namespace err {

// DB
const i32 DB_CONN_ERR = 100;
const i32 DB_PREPARE_ERR = 101;
const i32 DB_EXEC_ERR = 102;

} // namespace err

#endif

