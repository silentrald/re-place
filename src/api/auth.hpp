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
#include "config/logger.hpp"
#include "config/types.hpp"
#include "use-case/auth/login.hpp"
#include <cstdio>

namespace api {

template <typename UserRepo, typename SessionCache> class PostLogin {
public:
  opt_err init(use_case::auth::Login<UserRepo, SessionCache>* use_case
  ) noexcept {
    if (use_case == nullptr) {
      // TODO:
      return error{"value error", def_err_vals};
    }

    this->use_case = use_case;
    return null;
  }

  void
  operator()(const http::server::request& req, http::server::response& rep) {
    const char* username = req.get_body_parameter("username");
    const char* password = req.get_body_parameter("password");

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

private:
  use_case::auth::Login<UserRepo, SessionCache>* use_case = nullptr;
};

} // namespace api

#endif

