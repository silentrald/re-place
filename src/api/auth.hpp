/*============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-04-20
 *============================*/

#ifndef API_AUTH_HPP
#define API_AUTH_HPP

#include "api/asio/header.hpp"
#include "api/asio/request.hpp"
#include "api/asio/response.hpp"
#include "api/asio/server.hpp"
#include "entity/user.hpp"
#include "llhttp.h"
#include "service/auth.hpp"
#include "types.hpp"
#include "utils/logger/logger.hpp"
#include <cstring>

#define AUTH_TEMPLATE template <typename UserRepo, typename SessionRepo>
#define AUTH_SERVICE const service::Auth<UserRepo, SessionRepo>*
#define AUTH_LAMBDA(callback)                                                  \
  [](const void* data, const http::server::request& req,                       \
     http::server::response& res) noexcept {                                   \
    callback((AUTH_SERVICE)data, req, res);                                    \
  }
#define AUTH_ENDPOINT(function_name)                                           \
  AUTH_TEMPLATE void function_name(                                            \
      AUTH_SERVICE auth_service, const http::server::request& request,         \
      http::server::response& response                                         \
  ) noexcept

namespace api::auth {

// BUG: don't static_cast<void> response.content.copy, they can still throw from
// bad_allocation

const c8* const SESSION_COOKIE_NAME = "rp-session=";
inline const usize SESSION_COOKIE_NAME_LENGTH =
    std::strlen(SESSION_COOKIE_NAME);

AUTH_ENDPOINT(post_login) {
  const c8* username = request.get_body_parameter("username");
  const c8* password = request.get_body_parameter("password");

  expected<uuid, error_code> exp_id = auth_service->login(username, password);
  if (rp::is_error(exp_id)) {
    response.status = http::server::response::status_type::unauthorized;
    logger::error("Error code %u", exp_id.error());
    static_cast<void>(response.content.copy("Auth Failed"));
    return;
  }

  response.status = http::server::response::status_type::ok;

  http::server::header session_cookie{};
  session_cookie.name.copy("Set-Cookie");
  session_cookie.value.copy(SESSION_COOKIE_NAME);
  session_cookie.value.append(exp_id.value().get_string());
  session_cookie.value.append(";"); // TODO: Cookie attribs

  response.headers.push(std::move(session_cookie));
  static_cast<void>(response.content.copy("Logged In"));
}

AUTH_ENDPOINT(post_register) {
  const c8* username = request.get_body_parameter("username");
  const c8* password = request.get_body_parameter("password");

  error_code code = auth_service->register_user(username, password);
  if (rp::is_error(code)) {
    response.status = http::server::response::status_type::bad_request;
    logger::error("Error code %u", code);
    static_cast<void>(response.content.copy("Bad Request"));
    return;
  }

  response.status = http::server::response::status_type::ok;
  static_cast<void>(response.content.copy("Registered"));
}

AUTH_ENDPOINT(post_logout) {
  uuid session_id{};

  // TODO: Hide implementation details here, middleware here
  for (usize i = 0; i < request.headers.get_size(); ++i) {
    if (request.headers[i].name != "cookie") {
      continue;
    }

    const string& value = request.headers[i].value;
    usize k = 0;
    for (usize j = 0; j < value.get_size(); ++j) {
      if (value[j] != SESSION_COOKIE_NAME[k]) {
        k = 0;
        continue;
      }

      if (++k == SESSION_COOKIE_NAME_LENGTH) {
        session_id = value.c_str() + k;
        break;
      }
    }
    break;
  }

  if (session_id.is_empty()) {
    response.status = http::server::response::status_type::unauthorized;
    static_cast<void>(response.content.copy("Unauthorized"));
    return;
  }

  error_code code = auth_service->logout(session_id);
  if (rp::is_error(code)) {
    response.status = http::server::response::status_type::bad_request;
    logger::error("Error code %u", code);
    static_cast<void>(response.content.copy("Bad Request"));
    return;
  }

  response.status = http::server::response::status_type::ok;
  static_cast<void>(response.content.copy("Logged out"));
}

// TODO: Move this out of auth api
AUTH_ENDPOINT(get_user) {
  uuid session_id{};

  // TODO: Hide implementation details here, middleware here
  for (usize i = 0; i < request.headers.get_size(); ++i) {
    if (request.headers[i].name != "cookie") {
      continue;
    }

    const string& value = request.headers[i].value;
    usize k = 0;
    for (usize j = 0; j < value.get_size(); ++j) {
      if (value[j] != SESSION_COOKIE_NAME[k]) {
        k = 0;
        continue;
      }

      if (++k == SESSION_COOKIE_NAME_LENGTH) {
        session_id = value.c_str() + k;
        break;
      }
    }
    break;
  }

  if (session_id.is_empty()) {
    response.status = http::server::response::status_type::unauthorized;
    static_cast<void>(response.content.copy("Unauthorized"));
    return;
  }

  expected<entity::User, error_code> exp_user =
      auth_service->get_user(session_id);
  if (rp::is_error(exp_user)) {
    logger::warn("Error code %u", exp_user.error());
    response.status = http::server::response::status_type::unauthorized;
    static_cast<void>(response.content.copy("Unauthorized"));
    return;
  }

  response.status = http::server::response::status_type::ok;
  response.content.copy("[");
  response.content.append(exp_user.value().get_id().get_string(), uuid::SIZE);
  response.content.append("]: ");
  response.content.append(exp_user.value().get_username());
}

AUTH_TEMPLATE void
setup(http::server::server& server, AUTH_SERVICE auth_service) noexcept {
  server.add_route(http::server::router{
      .method = HTTP_POST,
      .path = "/api/auth/login",
      .data = auth_service,
      .endpoint = AUTH_LAMBDA(post_login)
  });

  server.add_route(http::server::router{
      .method = HTTP_POST,
      .path = "/api/auth/register",
      .data = auth_service,
      .endpoint = AUTH_LAMBDA(post_register)
  });

  server.add_route(http::server::router{
      .method = HTTP_POST,
      .path = "/api/auth/logout",
      .data = auth_service,
      .endpoint = AUTH_LAMBDA(post_logout)
  });

  server.add_route(http::server::router{
      .method = HTTP_GET,
      .path = "/api/user",
      .data = auth_service,
      .endpoint = AUTH_LAMBDA(get_user)
  });
}

} // namespace api::auth

#undef AUTH_ENDPOINT
#undef AUTH_LAMBDA
#undef AUTH_SERVICE
#undef AUTH_TEMPLATE

#endif
