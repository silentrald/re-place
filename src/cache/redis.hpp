/*============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-05-10
 *============================*/

#ifndef CACHE_REDIS_HPP
#define CACHE_REDIS_HPP

#include "config/types.hpp"
#include "hiredis.h"

namespace cache {

class RedisClient;

class RedisManager {
private:
  string host;
  i32 port = 0;

public:
  RedisManager() noexcept = default;
  RedisManager(const RedisManager&) noexcept = delete;
  RedisManager& operator=(const RedisManager&) noexcept = delete;

  // === Constructor === //

  [[nodiscard]] opt_err init(const c8* host, i32 port) noexcept;

  RedisManager(RedisManager&& rhs) noexcept = default;
  RedisManager& operator=(RedisManager&& rhs) noexcept = default;

  ~RedisManager() noexcept = default;

  // === Functions === //
  [[nodiscard]] exp_err<shared_ptr<RedisClient>> get_client() noexcept;
};

class RedisClient {
private:
  redisContext* ctx = nullptr;

public:
  friend RedisManager;

  RedisClient() noexcept = default;
  RedisClient(const RedisClient&) noexcept = delete;
  RedisClient& operator=(const RedisClient&) noexcept = delete;

  RedisClient(RedisClient&& rhs) noexcept;
  RedisClient& operator=(RedisClient&& rhs) noexcept;
  ~RedisClient() noexcept;

  // === Sync Functions === //

  // TODO: Look into what should be used here
  [[nodiscard]] opt_err
  set(const c8* key, const c8* value, u32 expiry) noexcept;
  [[nodiscard]] opt_err
  set(const string& key, const string& value, u32 expiry) noexcept;
  [[nodiscard]] exp_err<string> get(const c8* key) noexcept;
  [[nodiscard]] exp_err<string> get(const string& key) noexcept;
};

} // namespace cache

#endif

