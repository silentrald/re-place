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
#include "types.hpp"
#include "use-case/auth/login.hpp"
#include "use-case/auth/register.hpp"
#include "utils/logger/logger.hpp"
#include <cstdio>

namespace api {

// TODO: Change this
// template <typename UserRepo, typename SessionCache> class PostLogin {
template <typename UserRepo> class PostLogin {
public:
  explicit PostLogin(use_case::auth::Login<UserRepo>* use_case) noexcept {
    assert(use_case != nullptr);
    this->use_case = use_case;
  }

  void operator()(
      const http::server::request& request, http::server::response& response
  ) noexcept {
    const c8* username = request.get_body_parameter("username");
    const c8* password = request.get_body_parameter("password");

    error_code err = this->use_case->execute(username, password);
    if (rp::is_error(err)) {
      response.status = http::server::response::status_type::unauthorized;
      logger::error("Error code %u", err);
      static_cast<void>(response.content.copy("Auth Failed"));
      return;
    }

    response.status = http::server::response::status_type::ok;
    static_cast<void>(response.content.copy("Logged In"));
  }

private:
  // use_case::auth::Login<UserRepo, SessionCache>* use_case = nullptr;
  use_case::auth::Login<UserRepo>* use_case = nullptr;
};

// TODO: Change this
template <typename UserRepo> class PostRegister {
public:
  explicit PostRegister(use_case::auth::Register<UserRepo>* use_case) {
    assert(use_case != nullptr);
    this->use_case = use_case;
  }

  void operator()(
      const http::server::request& request, http::server::response& response
  ) noexcept {
    const c8* username = request.get_body_parameter("username");
    const c8* password = request.get_body_parameter("password");

    error_code err = this->use_case->execute(username, password);
    if (rp::is_error(err)) {
      response.status = http::server::response::status_type::bad_request;
      logger::error("Error code %u", err);
      static_cast<void>(response.content.copy("TODO"));
      return;
    }

    response.status = http::server::response::status_type::ok;
    static_cast<void>(response.content.copy("Registered"));
  }

private:
  use_case::auth::Register<UserRepo>* use_case = nullptr;
};

} // namespace api

#endif
