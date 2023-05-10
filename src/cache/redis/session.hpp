/*============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-05-10
 *============================*/

#ifndef CACHE_REDIS_SESSION_HPP
#define CACHE_REDIS_SESSION_HPP

#include "cache/redis.hpp"
#include "cache/session.hpp"
#include "config/types.hpp"
#include "entity/user.hpp"

namespace cache {

class RedisSession : public Session<RedisSession> {
public:
  RedisSession() noexcept = default;
  RedisSession(const RedisSession&) noexcept = delete;
  RedisSession& operator=(const RedisSession&) noexcept = delete;
  RedisSession(RedisSession&&) noexcept = default;
  RedisSession& operator=(RedisSession&&) noexcept = default;
  ~RedisSession() noexcept = default;

  opt_err init(RedisManager* redis) noexcept;

  [[nodiscard]] exp_err<string> set_session_impl(const entity::User& user
  ) noexcept;

  [[nodiscard]] exp_err<entity::User> get_session_impl(const string& session_id
  ) noexcept;

private:
  RedisManager* redis = nullptr;
};

} // namespace cache

#endif

