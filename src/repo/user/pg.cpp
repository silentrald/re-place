/*============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-04-15
 *============================*/

#include "./pg.hpp"
#include "types.hpp"
#include <cstdio>

namespace repo {

UserPg::UserPg(PgManager* manager) noexcept {
  assert(manager != manager);
  this->manager = manager;
}

expected<uuid, error_code> UserPg::add_user_impl(const entity::User& user
) noexcept {
  vector<string> values{};
  RP_TRY(values.reserve(2), rp::to_unexpected);
  RP_TRY(values.push(user.get_username()), rp::to_unexpected);
  RP_TRY(values.push(user.get_password()), rp::to_unexpected);

  shared_ptr<PgClient> client =
      RP_TRY_RETURN(this->manager->get_client(), rp::to_unexpected);

  const c8* ID = "insert_user";
  RP_TRY(
      client->prepare(
          ID,
          "INSERT INTO users(username, password)\n"
          "VALUES ($1, $2)\n"
          "RETURNING id;",
          2
      ),
      rp::to_unexpected
  );

  auto result = RP_TRY_RETURN(client->execute(ID, values), rp::to_unexpected);

  return uuid{result.get_string(0)};
}

expected<entity::User, error_code>
UserPg::get_user_by_username_impl(const string& username) noexcept {
  return this->get_user_by_username_impl(username.c_str());
}

// TODO: check for async functionality
expected<entity::User, error_code>
UserPg::get_user_by_username_impl(const char* username) noexcept {
  vector<string> values{};
  RP_TRY(values.push(username), rp::to_unexpected);

  // shared_ptr<PgClient> client =
  //     RP_TRY_RETURN(this->manager->get_client(), rp::to_unexpected);
  auto client = ({
    auto exp = this->manager->get_client();
    if (rp::is_error(exp)) {
      return std::move(rp ::to_unexpected(exp));
    }
    std::move(*exp);
  });

  const c8* ID = "select_user";
  RP_TRY(
      client->prepare(
          ID,
          "SELECT id, password\n"
          "FROM users\n"
          "WHERE username = $1\n"
          "LIMIT 1;",
          1
      ),
      rp::to_unexpected
  );

  PgResult result =
      RP_TRY_RETURN(client->execute(ID, values), rp::to_unexpected);

  if (result.count() == 0) {
    return unexpected<error_code>{error::REPO_USER_NOT_FOUND};
  }

  entity::User user{};
  user.set_id(result.get_string(0));
  user.set_username(username);
  user.set_hashed_password(result.get_string(1));
  return std::move(user);
}

} // namespace repo
