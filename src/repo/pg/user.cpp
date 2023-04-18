/*============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-04-15
 *============================*/

#include "./user.hpp"
#include "config/types.hpp"
#include "entity/user.hpp"

namespace repo {

types::exp_err<entity::User>
UserPg::get_user_by_username_impl(const types::string& username) noexcept {
  entity::UserBuilder builder{};
  builder.set_id("id");
  builder.set_username(username);
  builder.set_password("password");
  return builder.build();
}

types::exp_err<entity::User>
UserPg::get_user_by_username_impl(const char* username) noexcept {
  entity::UserBuilder builder{};
  builder.set_id("id");
  builder.set_username(username);
  builder.set_password("password");
  return builder.build();
}

} // namespace repo

