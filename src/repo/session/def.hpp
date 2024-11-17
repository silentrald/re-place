/*============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2024-11-17
 *============================*/

#ifndef REPLACE_REPO_SESSION_HPP
#define REPLACE_REPO_SESSION_HPP

#include "entity/user.hpp"
#include "types.hpp"

namespace repo {

template <typename Derived> class Session {

public:
  friend Derived;

  // === Set === //

  /**
   * @return the session id
   **/
  [[nodiscard]] expected<uuid, error_code> set_session(const entity::User& user
  ) noexcept {
    return static_cast<Derived*>(this)->set_session_impl(user);
  }

  // === Get === //

  [[nodiscard]] expected<entity::User, error_code>
  get_user(const uuid& session_id) noexcept {
    return static_cast<Derived*>(this)->get_user_impl(session_id);
  }

  // === Delete === //

  [[nodiscard]] error_code remove_session(const uuid& session_id) noexcept {
    return static_cast<Derived*>(this)->remove_session_impl(session_id);
  }

  // === Deleted Impls === //

  [[nodiscard]] expected<uuid, error_code>
  set_session_impl(const entity::User& user) noexcept = delete;
  [[nodiscard]] expected<entity::User, error_code>
  get_user_impl(const uuid& session_id) noexcept = delete;
  [[nodiscard]] error_code remove_session_impl(const uuid& session_id
  ) noexcept = delete;
};

} // namespace repo

#endif
