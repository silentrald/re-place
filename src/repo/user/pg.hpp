/*============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-04-15
 *============================*/

#ifndef REPLACE_REPO_USER_PG_HPP
#define REPLACE_REPO_USER_PG_HPP

#include "./def.hpp"
#include "entity/user.hpp"
#include "repo/pg.hpp"

namespace repo {

class UserPg : public User<UserPg> {
public:
  explicit UserPg(PgManager* manager) noexcept;

  // === Function Overrides === //

  [[nodiscard]] expected<uuid, error_code>
  add_user_impl(const entity::User& user) noexcept;

  [[nodiscard]] expected<entity::User, error_code>
  get_user_by_username_impl(const string& username) noexcept;

  [[nodiscard]] expected<entity::User, error_code>
  get_user_by_username_impl(const c8* username) noexcept;

private:
  PgManager* manager = nullptr;
};

} // namespace repo

#endif
