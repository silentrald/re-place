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
#include "ds/types.hpp"
#include "llhttp.h"
#include "repo/pg/user.hpp"
#include "use-case/auth/login.hpp"
#include <cstdio>

namespace api {

template <typename AuthLoginUseCase>
http::server::router
get_auth_login(use_case::auth::Login<AuthLoginUseCase>* login_uc) noexcept {
  return http::server::router{
      .method = HTTP_GET,
      .path = "/api/login",
      .endpoint = [login_uc](
                      const http::server::request& req,
                      http::server::response& rep
                  ) { // TODO: Wrap this to mock
        types::err_code ec{};
        types::string username{};
        types::string password{};
        ec = username.copy(req.get_parameter("username"));
        ec = password.copy(req.get_parameter("password"));

        auto err = login_uc->execute(username, password);
        if (!err) {
          rep.status = http::server::response::status_type::ok;
          ec = rep.content.copy("Logged In");
        } else {
          rep.status = http::server::response::status_type::unauthorized;
          printf(
              "Msg: %s\n> %s:%d\n", err->get_msg(), err->get_def_file(),
              err->get_def_line()
          ); // TODO: Logger
          ec = rep.content.copy("Auth Failed");
        }
      }};
}

} // namespace api

#endif

