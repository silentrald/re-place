
#include "./auth.hpp"
#include "api/asio/response.hpp"
#include "api/asio/router.hpp"
#include "config/types.hpp"
#include "ds/string.hpp"
#include "ds/types.hpp"
#include <cstdio>

namespace api {

http::server::router get_auth_login() {
  return http::server::router{
      .method = HTTP_GET,
      .path = "/api/login",
      // TODO: Edit this to pass scoped variables
      .endpoint = [](const http::server::request& req,
                     http::server::response& rep) {
        types::err_code ec{};
        repo::UserPg user_repo{};
        use_case::auth::Login<repo::UserPg> login_uc{&user_repo};

        const char* username_ptr = req.get_parameter<const char*>("username");
        const char* password_ptr = req.get_parameter<const char*>("password");

        types::string username{};
        types::string password{};
        ec = username.copy(username_ptr);
        ec = password.copy(password_ptr);

        auto err = login_uc.execute(username, password);
        if (!err) {
          rep.status = http::server::response::status_type::ok;
          ec = rep.content.copy("Logged In");
        } else {
          rep.status = http::server::response::status_type::unauthorized;
          ec = rep.content.copy("Auth Failed");
        }
        ec = rep.headers.resize(1);
        ec = rep.headers[0].name.copy("Content-Length");
        ec =
            rep.headers[0].value.copy(std::to_string(rep.content.size()).c_str()
            );
      }};
}

} // namespace api

