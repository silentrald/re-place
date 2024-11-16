/*============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-04-20
 *============================*/

#ifndef API_AUTH_HPP
#define API_AUTH_HPP

#include "api/asio/request.hpp"
#include "api/asio/response.hpp"
#include "api/asio/server.hpp"
#include "service/auth.hpp"
#include "types.hpp"
#include "utils/logger/logger.hpp"

namespace api::auth {

template <typename UserRepo>
void post_login(
    const service::Auth<UserRepo>* auth_service,
    const http::server::request& request, http::server::response& response
) noexcept {
  const c8* username = request.get_body_parameter("username");
  const c8* password = request.get_body_parameter("password");

  error_code err = auth_service->login(username, password);
  if (rp::is_error(err)) {
    response.status = http::server::response::status_type::unauthorized;
    logger::error("Error code %u", err);
    static_cast<void>(response.content.copy("Auth Failed"));
    return;
  }

  response.status = http::server::response::status_type::ok;
  static_cast<void>(response.content.copy("Logged In"));
}

template <typename UserRepo>
void post_register(
    const service::Auth<UserRepo>* auth_service,
    const http::server::request& request, http::server::response& response
) noexcept {
  const c8* username = request.get_body_parameter("username");
  const c8* password = request.get_body_parameter("password");

  error_code err = auth_service->execute(username, password);
  if (rp::is_error(err)) {
    response.status = http::server::response::status_type::bad_request;
    logger::error("Error code %u", err);
    static_cast<void>(response.content.copy("TODO"));
    return;
  }

  response.status = http::server::response::status_type::ok;
  static_cast<void>(response.content.copy("Registered"));
}

template <typename UserRepo>
void setup(
    http::server::server& server, const service::Auth<UserRepo>* auth_service
) noexcept {
  server.add_route(http::server::router{
      .method = HTTP_POST,
      .path = "/api/auth/login",
      .data = auth_service,
      .endpoint =
          [](const void* data, const http::server::request& req,
             http::server::response& res) noexcept {
            post_login<UserRepo>(
                (const service::Auth<UserRepo>*)data, req, res
            );
          }
  });

  server.add_route(http::server::router{
      .method = HTTP_POST,
      .path = "/api/auth/register",
      .data = auth_service,
      .endpoint =
          [](const void* data, const http::server::request& req,
             http::server::response& res) noexcept {
            post_register<UserRepo>(
                (const service::Auth<UserRepo>*)data, req, res
            );
          }
  });
}

} // namespace api::auth

#endif
