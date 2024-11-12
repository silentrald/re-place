/*============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2024-11-11
 *============================*/

#ifndef UTILS_CRYPTO_HPP
#define UTILS_CRYPTO_HPP

#include "types.hpp"

namespace crypto {

[[nodiscard]] expected<string, error_code> hash_password(const string& password
) noexcept;
[[nodiscard]] expected<string, error_code> hash_password(const c8* password
) noexcept;
[[nodiscard]] expected<string, error_code>
hash_password(const c8* password, usize size) noexcept;

[[nodiscard]] bool
verify_password(const string& hash, const string& password) noexcept;
[[nodiscard]] bool
verify_password(const string& hash, const c8* password) noexcept;
[[nodiscard]] bool
verify_password(const string& hash, const c8* password, usize size) noexcept;

} // namespace crypto

#endif
