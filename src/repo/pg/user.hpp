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
#include "repo/pg.hpp"
#include "repo/user.hpp"

namespace repo {

class UserPg : public User<UserPg> {
public:
  UserPg() noexcept = default;
  UserPg(const UserPg&) noexcept = delete;
  UserPg& operator=(const UserPg&) noexcept = delete;
  UserPg(UserPg&& rhs) noexcept = default;
  UserPg& operator=(UserPg&& rhs) noexcept = default;
  ~UserPg() noexcept = default;

  // === Constructor === //
  opt_err init(PgManager* repo) noexcept;

  // === Create === //
  string add_user_impl(const entity::User& user) noexcept;

  // === Read === //
  exp_err<entity::User> get_user_by_username_impl(const string& username
  ) noexcept;
  exp_err<entity::User> get_user_by_username_impl(const char* username
  ) noexcept;

private:
  PgManager* repo = nullptr;
};

} // namespace repo

#endif

