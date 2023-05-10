/*============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-05-10
 *============================*/

#include "cache/redis/session.hpp"
#include "cache/redis.hpp"
#include "config/types.hpp"
#include "ds/macro.hpp"
#include "entity/user.hpp"

namespace cache {

const i32 EXPIRY = 60 * 20; // 20 mins

opt_err RedisSession::init(RedisManager* redis) noexcept {
  if (redis == nullptr) {
    return error{"value null", def_err_vals};
  }

  this->redis = redis;
  return null;
}

// TODO: Flatbuffers
exp_err<string> RedisSession::set_session_impl(const entity::User& user
) noexcept {
  string key{};
  string value{};

  // TODO: randomly generated
  try_opt_unexp(key.copy("0123456789abcdef"));

  try_opt_unexp(value.copy(user.get_id()));
  try_opt_unexp(value.push_back(':'));
  try_opt_unexp(value.append(user.get_username()));

  auto client = try_exp_unexp(this->redis->get_client());
  try_opt_unexp(client->set(key, value, EXPIRY));

  return key;
}

exp_err<entity::User> RedisSession::get_session_impl(const string& session_id
) noexcept {
  auto client = try_exp_unexp(this->redis->get_client());
  // TODO: What if null?
  auto user_str = try_exp_unexp(client->get(session_id));

  for (i32 i = 0; i < user_str.length(); ++i) {
    if (user_str[i] == '/') {
      entity::UserBuilder builder{};
      string substr = try_exp_unexp(user_str.substr(0, i));
      try_opt_unexp(builder.set_id(substr));

      substr = try_exp_unexp(user_str.substr(i + 1));
      try_opt_unexp(builder.set_username(substr));

      return builder.build();
    }
  }

  return unexp_err{error{"invalid session string", def_err_vals}};
}

} // namespace cache

