/*============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2024-11-03
 *============================*/

#include "./test.hpp"
#include "ds/types.hpp"
#include "entity/user.hpp"
#include "types.hpp"

namespace repo {

// NOLINTNEXTLINE
expected<uuid, error_code> UserTest::add_user_impl(const entity::User& user
) noexcept {
  return uuid{"00000000-0000-0000-0000-000000000000"};
}

expected<entity::User, error_code>
UserTest::get_user_by_username_impl(const string& username) noexcept { // NOLINT
  entity::User user{};
  user.set_id("00000000-0000-0000-0000-000000000000");
  RP_TRY(user.set_username(username), ds::to_unexpected);
  RP_TRY(user.set_password("password"), ds::to_unexpected);
  return std::move(user);
}

expected<entity::User, error_code>
UserTest::get_user_by_username_impl(const c8* username) noexcept { // NOLINT
  entity::User user{};
  user.set_id("00000000-0000-0000-0000-000000000000");
  RP_TRY(user.set_username(username), ds::to_unexpected);
  RP_TRY(user.set_password("password"), ds::to_unexpected);
  return std::move(user);
}

} // namespace repo
