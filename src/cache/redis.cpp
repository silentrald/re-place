/*============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-05-10
 *============================*/

#include "./redis.hpp"
#include "config/types.hpp"
#include "ds-error/types.hpp"
#include "ds/macro.hpp"
#include "hiredis.h"
#include "read.h"

namespace cache {

// === RedisManager === //
opt_err RedisManager::init(const c8* host, i32 port) noexcept {
  try_opt(this->host.copy(host));
  this->port = port;
  return null;
}

exp_err<shared_ptr<RedisClient>> RedisManager::get_client() noexcept {
  shared_ptr<RedisClient> client{};
  try_opt_unexp(client.init());

  client->ctx = redisConnect(this->host.c_str(), this->port);
  if (client->ctx == nullptr) {
    return unexp_err{error{ds::DS_ERROR_BAD_ALLOC, def_err_vals}};
  }

  if (client->ctx->err) {
    // TODO: Redis errors id
    return unexp_err{error{
        client->ctx->errstr, 10, "Error in redis connection", def_err_vals}};
  }

  return client;
}

// === RedisManager === //
RedisClient::RedisClient(RedisClient&& rhs) noexcept : ctx(rhs.ctx) {
  rhs.ctx = nullptr;
}

RedisClient& RedisClient::operator=(RedisClient&& rhs) noexcept {
  if (this == &rhs) {
    return *this;
  }

  this->ctx = rhs.ctx;

  rhs.ctx = nullptr;

  return *this;
}

RedisClient::~RedisClient() noexcept {
  if (this->ctx) {
    redisFree(this->ctx);
    this->ctx = nullptr;
  }
}

opt_err RedisClient::set(const c8* key, const c8* value, u32 expiry) noexcept {
  auto* reply = static_cast<redisReply*>(
      redisCommand(this->ctx, "SET %s %s EX %u", key, value, expiry)
  );

  if (reply->type == REDIS_REPLY_ERROR) {
    // TODO:
    error err{reply->str, 12, "Could not set in redis", def_err_vals};
    freeReplyObject(reply);
    return err;
  }

  freeReplyObject(reply);
  return null;
}

opt_err
RedisClient::set(const string& key, const string& value, u32 expiry) noexcept {
  return this->set(key.c_str(), value.c_str(), expiry);
}

exp_err<string> RedisClient::get(const c8* key) noexcept {
  string value{};
  auto* reply =
      static_cast<redisReply*>(redisCommand(this->ctx, "GET %s", key));

  try_opt_unexp(value.copy(reply->str));
  if (reply->type == REDIS_REPLY_ERROR) {
    freeReplyObject(reply);
    // TODO:
    return unexp_err{
        error{value.data(), 11, "Could not get in redis", def_err_vals}};
  }

  freeReplyObject(reply);
  return value;
}

exp_err<string> RedisClient::get(const string& key) noexcept {
  return this->get(key.c_str());
}

} // namespace cache

