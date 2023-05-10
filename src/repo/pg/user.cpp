/*============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-04-15
 *============================*/

#include "./user.hpp"
#include "config/types.hpp"
#include "ds/macro.hpp"
#include "ds/types.hpp"
#include "entity/user.hpp"

namespace repo {

const char* const GUBU_ID = "gubu";
const char* const GUBU_QRY = "SELECT id, username, password\n"
                             "FROM users\n"
                             "WHERE username = $1\n"
                             "LIMIT 1;";
const i32 GUBU_PARAMS = 1;
const i32 GUBU_F_ID = 0;
const i32 GUBU_F_USERNAME = 1;
const i32 GUBU_F_PASSWORD = 2;

opt_err UserPg::init(PgManager* repo) noexcept {
  if (repo == nullptr) {
    // TODO:
    return error{"init null", def_err_vals};
  }

  this->repo = repo;
  return null;
}

exp_err<entity::User> UserPg::get_user_by_username_impl(const string& username
) noexcept {
  return this->get_user_by_username_impl(username.c_str());
}

exp_err<entity::User> UserPg::get_user_by_username_impl(const char* username
) noexcept {
  vector<string> values{};
  try_opt_unexp(values.push_back(username));

  auto client = try_exp_unexp(this->repo->get_client());
  try_opt_unexp(client->prepare(GUBU_ID, GUBU_QRY, GUBU_PARAMS));
  auto res = try_exp_unexp(client->execute(GUBU_ID, values));
  if (res.count() == 0) {
    return unexp_err{error{"User does not exist", def_err_vals}};
  }

  entity::UserBuilder builder{};
  try_opt_unexp(builder.set_id(res.get(GUBU_F_ID)));
  try_opt_unexp(builder.set_username(res.get(GUBU_F_USERNAME)));
  try_opt_unexp(builder.set_password(res.get(GUBU_F_PASSWORD)));
  return builder.build();
}

} // namespace repo

