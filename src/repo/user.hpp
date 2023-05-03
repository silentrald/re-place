/*============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-04-15
 *============================*/

#ifndef REPO_USER_HPP
#define REPO_USER_HPP

#include "config/types.hpp"
#include "entity/user.hpp"

namespace repo {

template <typename T> class User {
public:
  friend T;

  // === Create === //
  string add_user(const entity::User& user) noexcept {
    return static_cast<T*>(this)->add_user_impl(user);
  }
  string add_user_impl(const entity::User& user) noexcept = delete;

  // === Read === //
  exp_err<entity::User> get_user_by_username(const string& username) noexcept {
    return static_cast<T*>(this)->get_user_by_username_impl(username);
  }
  exp_err<entity::User> get_user_by_username(const char* username) noexcept {
    return static_cast<T*>(this)->get_user_by_username_impl(username);
  }
  exp_err<entity::User> get_user_by_username_impl(const string& username
  ) noexcept = delete;
  exp_err<entity::User> get_user_by_username_impl(const char* username
  ) noexcept = delete;

  // === Update === //

  // === Delete === //
};

} // namespace repo

#endif

