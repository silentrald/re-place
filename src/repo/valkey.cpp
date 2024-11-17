/*============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2024-11-17
 *============================*/

#include "./valkey.hpp"
#include "utils/logger/logger.hpp"
#include <bits/types/struct_timeval.h>

namespace repo {

// === ValkeyClient === //

ValkeyClient::ValkeyClient(ValkeyClient&& rhs) noexcept : context(rhs.context) {
  rhs.context = nullptr;
}

ValkeyClient& ValkeyClient::operator=(ValkeyClient&& other) noexcept {
  if (this == &other) {
    return *this;
  }

  this->context = other.context;
  other.context = nullptr;

  return *this;
}

ValkeyClient::~ValkeyClient() noexcept {
  if (this->context != nullptr) {
    valkeyFree(this->context);
    this->context = nullptr;
  }
}

// === SET === //

/**
 * @param expiry - expiration in seconds
 **/
[[nodiscard]] error_code ValkeyClient::set_string(
    const c8* key, const c8* value, u32 expiry
) const noexcept {
  auto* reply = static_cast<valkeyReply*>(
      valkeyCommand(this->context, "SET %s %s EX %u", key, value, expiry)
  );
  if (reply == nullptr) {
    logger::error("Valkey set error: %s", this->context->errstr);
    return error::VALKEY_COMMAND_ERROR;
  }

  freeReplyObject(reply);
  return error::OK;
}

// === GET === //

[[nodiscard]] expected<string, error_code>
ValkeyClient::get_string(const c8* key) const noexcept {
  auto* reply =
      static_cast<valkeyReply*>(valkeyCommand(this->context, "GET %s", key));
  if (reply == nullptr) {
    logger::error("Valkey get error: %s", this->context->errstr);
    return unexpected<error_code>(error::VALKEY_COMMAND_ERROR);
  }

  if (reply->type != VALKEY_REPLY_STRING) {
    logger::error(
        "Valkey invalid type; %d != %d", reply->type, VALKEY_REPLY_STRING
    );
    freeReplyObject(reply);

    return unexpected<error_code>(error::VALKEY_TYPE_ERROR);
  }

  string str{};
  RP_TRY(str.copy(reply->str), rp::to_unexpected);
  freeReplyObject(reply);

  return std::move(str);
}

// === DELETE === //

[[nodiscard]] error_code ValkeyClient::remove(const c8* key) const noexcept {
  auto* reply =
      static_cast<valkeyReply*>(valkeyCommand(this->context, "DEL %s", key));
  if (reply == nullptr) {
    logger::error("Valkey remove error: %s", this->context->errstr);
    return error::VALKEY_COMMAND_ERROR;
  }

  bool removed = reply->integer == 1LL;
  freeReplyObject(reply);
  return removed ? error::OK : error::VALKEY_NONE_DELETED_ERROR;
}

// === Init === //

[[nodiscard]] error_code
ValkeyClient::init(const string& host, i32 port) noexcept {
  timeval timeout{1, 500'000}; // 1.5 seconds
  this->context = valkeyConnectWithTimeout(host.c_str(), port, timeout);
  if (this->context == nullptr) {
    return error::BAD_ALLOCATION;
  }

  if (this->context->err) {
    logger::error("Valkey connection error: %s", context->errstr);
    return error::VALKEY_CONNECTION_ERROR;
  }

  return error::OK;
}

// === ValkeyManager === //

[[nodiscard]] error_code
ValkeyManager::init(const c8* host, i32 port) noexcept {
  RP_TRY(this->host.copy(host));
  this->port = port;
  return error::OK;
}

[[nodiscard]] error_code
ValkeyManager::init(const string& host, i32 port) noexcept {
  RP_TRY(this->host.copy(host));
  this->port = port;
  return error::OK;
}

[[nodiscard]] expected<ValkeyClient, error_code>
ValkeyManager::get_client() noexcept {
  ValkeyClient client{};
  RP_TRY(client.init(this->host, this->port), rp::to_unexpected);
  return std::move(client);
}

} // namespace repo
