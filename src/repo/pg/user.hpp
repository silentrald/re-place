/*============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-04-15
 *============================*/

#ifndef REPO_PG_USER_HPP
#define REPO_PG_USER_HPP

#include "config/types.hpp"
#include "entity/user.hpp"
#include "repo/user.hpp"

namespace repo {

class UserPg : public User<UserPg> {
private:
public:
  UserPg() noexcept = default;
  UserPg(const UserPg&) noexcept = delete;
  UserPg& operator=(const UserPg&) noexcept = delete;

  // === Constructor === //
  types::opt_err init() noexcept;

  UserPg(UserPg&& rhs) noexcept = default;
  UserPg& operator=(UserPg&& rhs) noexcept = default;

  ~UserPg() noexcept = default;

  // === Create === //
  types::string add_user_impl(const entity::User& user) noexcept;

  // === Read === //
  types::exp_err<entity::User>
  get_user_by_username_impl(const types::string& username) noexcept;
  types::exp_err<entity::User> get_user_by_username_impl(const char* username
  ) noexcept;
};

} // namespace repo

#endif

