/*============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2024-11-17
 *============================*/

#ifndef REPLACE_REPO_SESSION_VALKEY_HPP
#define REPLACE_REPO_SESSION_VALKEY_HPP

#include "./def.hpp"
#include "repo/valkey.hpp"

namespace repo {

class SessionValkey : public Session<SessionValkey> {
public:
  explicit SessionValkey(ValkeyManager* manager) noexcept;

  [[nodiscard]] expected<uuid, error_code>
  set_session_impl(const entity::User& user) noexcept;

  [[nodiscard]] expected<entity::User, error_code>
  get_user_impl(const uuid& session_id) noexcept;

  [[nodiscard]] error_code remove_session_impl(const uuid& session_id) noexcept;

private:
  ValkeyManager* manager = nullptr;
};

} // namespace repo

#endif
