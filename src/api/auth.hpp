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
#include "utils/logger/logger.hpp"
#include <cstdio>

namespace api {

// TODO: Change this
// template <typename UserRepo, typename SessionCache> class PostLogin {
template <typename UserRepo> class PostLogin {
public:
  explicit PostLogin(use_case::auth::Login<UserRepo>* use_case) {
    assert(use_case != nullptr);
    this->use_case = use_case;
  }

  void
  operator()(const http::server::request& req, http::server::response& rep) {
    const char* username = req.get_body_parameter("username");
    const char* password = req.get_body_parameter("password");

    auto err = this->use_case->execute(username, password);
    if (err) {
      rep.status = http::server::response::status_type::unauthorized;
      logger::error("Error code %u", err);
      static_cast<void>(rep.content.copy("Auth Failed"));
      return;
    }

    rep.status = http::server::response::status_type::ok;
    static_cast<void>(rep.content.copy("Logged In"));
  }

private:
  // use_case::auth::Login<UserRepo, SessionCache>* use_case = nullptr;
  use_case::auth::Login<UserRepo>* use_case = nullptr;
};

} // namespace api

#endif
