/*============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2024-11-17
 *============================*/

#ifndef REPLACE_REPO_VALKEY_HPP
#define REPLACE_REPO_VALKEY_HPP

#include "types.hpp"
#include "valkey.h"

namespace repo {

class ValkeyManager;

class ValkeyClient {
public:
  friend ValkeyManager;

  ValkeyClient() noexcept = default;

  ValkeyClient(const ValkeyClient&) noexcept = delete;
  ValkeyClient& operator=(const ValkeyClient&) noexcept = delete;

  ValkeyClient(ValkeyClient&& rhs) noexcept;
  ValkeyClient& operator=(ValkeyClient&& other) noexcept;

  ~ValkeyClient() noexcept;

  // === SET === //

  /**
   * @param expiry - expiration in seconds
   **/
  [[nodiscard]] error_code
  set_string(const c8* key, const c8* value, u32 expiry) const noexcept;

  // === GET === //

  [[nodiscard]] expected<string, error_code> get_string(const c8* key
  ) const noexcept;

  // === DELETE === //

  [[nodiscard]] error_code remove(const c8* key) const noexcept;

private:
  valkeyContext* context;

  // === Init === //

  [[nodiscard]] error_code init(const string& host, i32 port) noexcept;
};

class ValkeyManager {
public:
  ValkeyManager() noexcept = default;

  [[nodiscard]] error_code init(const c8* host, i32 port) noexcept;
  [[nodiscard]] error_code init(const string& host, i32 port) noexcept;

  [[nodiscard]] expected<ValkeyClient, error_code> get_client() noexcept;

private:
  string host;
  i32 port = 0;
};

} // namespace repo

#endif
