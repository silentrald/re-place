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
#include "ds/optional.hpp"
#include "ds/shared_ptr.hpp"
#include "ds/string.hpp"
#include "ds/types.hpp"
#include "ds/unique_ptr.hpp"
#include "ds/vector.hpp"
#include <cstdint>

namespace types {

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
template <typename T> using vector = ds::vector<T>;
template <typename T> using unique_ptr = ds::unique_ptr<T>;
template <typename T> using shared_ptr = ds::shared_ptr<T>;

const auto null = ds::null;

// Error Handling
using err_code = ds::err_code;

using error = ds::error;
using opt_err = ds::opt_err;
template <typename T> using exp_err = ds::exp_err<T>;
using unexp_err = ds::unexpected<error>;

using namespace ds;

} // namespace types

#endif

