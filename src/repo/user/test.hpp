/*============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2024-11-03
 *============================*/

#ifndef REPLACE_REPO_USER_TEST_HPP
#define REPLACE_REPO_USER_TEST_HPP

#include "./def.hpp"
#include "entity/user.hpp"
#include "types.hpp"

namespace repo {

class UserTest : public User<UserTest> {
public:
  [[nodiscard]] expected<uuid, error_code>
  add_user_impl(const entity::User& user) noexcept;

  [[nodiscard]] expected<entity::User, error_code>
  get_user_by_username_impl(const string& username) noexcept;

  [[nodiscard]] expected<entity::User, error_code>
  get_user_by_username_impl(const c8* username) noexcept;
};

} // namespace repo

#endif
