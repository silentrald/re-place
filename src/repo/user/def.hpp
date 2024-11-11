/*============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-04-15
 *============================*/

#ifndef REPLACE_REPO_USER_HPP
#define REPLACE_REPO_USER_HPP

#include "entity/user.hpp"
#include "types.hpp"

namespace repo {

template <typename Derived> class User {
public:
  friend Derived;

  // === Create === //

  [[nodiscard]] expected<uuid, error_code> add_user(const entity::User& user
  ) noexcept {
    return static_cast<Derived*>(this)->add_user_impl(user);
  }

  // === Read === //

  [[nodiscard]] expected<entity::User, error_code>
  get_user_by_username(const string& username) noexcept {
    return static_cast<Derived*>(this)->get_user_by_username_impl(username);
  }
  [[nodiscard]] expected<entity::User, error_code>
  get_user_by_username(const c8* username) noexcept {
    return static_cast<Derived*>(this)->get_user_by_username_impl(username);
  }

  // === Update === //

  // === Delete === //

  // === Deleted Impls === //

  [[nodiscard]] expected<uuid, error_code>
  add_user_impl(const entity::User& user) noexcept = delete;
  [[nodiscard]] expected<entity::User, error_code>
  get_user_by_username_impl(const string& username) noexcept = delete;
  [[nodiscard]] expected<entity::User, error_code>
  get_user_by_username_impl(const c8* username) noexcept = delete;
};

} // namespace repo

#endif
