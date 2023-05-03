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
#include "api/asio/router.hpp"
#include "config/logger.hpp"
#include "config/types.hpp"
#include "ds/types.hpp"
#include "llhttp.h"
#include "repo/pg/user.hpp"
#include "use-case/auth/login.hpp"
#include <cstdio>

namespace api {

template <typename UserRepo> class GetAuthLogin {
private:
  use_case::auth::Login<UserRepo>* use_case = nullptr;

public:
  // NOLINTNEXTLINE
  GetAuthLogin(use_case::auth::Login<UserRepo>* use_case) noexcept
      : use_case(use_case) {}

  void
  operator()(const http::server::request& req, http::server::response& rep) {
    const char* username = req.get_parameter("username");
    const char* password = req.get_parameter("password");

    auto err = this->use_case->execute(username, password);
    if (err) {
      rep.status = http::server::response::status_type::unauthorized;
      logger::error(*err);
      static_cast<void>(rep.content.copy("Auth Failed"));
      return;
    }

    rep.status = http::server::response::status_type::ok;
    static_cast<void>(rep.content.copy("Logged In"));
  }
};

} // namespace api

#endif

