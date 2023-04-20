
#include "./auth.hpp"
#include "api/asio/response.hpp"
#include "api/asio/router.hpp"
#include "config/types.hpp"
#include "ds/string.hpp"
#include "ds/types.hpp"
#include <cstdio>

namespace api {

const char* const USER = "sample_user";
const char* const PASS = "password";
const char* const DB = "sample_db";
const char* const HOST = "127.0.0.1";
const char* const PORT = "5432";

http::server::router get_auth_login() {
  return http::server::router{
      .method = HTTP_GET,
      .path = "/api/login",
      // TODO: Edit this to pass scoped variables
      .endpoint = [](const http::server::request& req,
                     http::server::response& rep) {
        printf("GET /api/login\n");

        // TODO: Another wrapper for this
        types::err_code ec{};
        repo::PgManager repo{};
        repo.init(USER, PASS, DB, HOST, PORT);
        repo::UserPg user_repo{&repo};
        use_case::auth::Login<repo::UserPg> login_uc{&user_repo};

        types::string username =
            std::move(req.get_parameter("username").value());
        types::string password =
            std::move(req.get_parameter("password").value());

        auto err = login_uc.execute(username, password);
        if (!err) {
          rep.status = http::server::response::status_type::ok;
          ec = rep.content.copy("Logged In");
        } else {
          rep.status = http::server::response::status_type::unauthorized;
          printf(
              "Msg: %s\n> %s:%d\n", err->get_msg(), err->get_def_file(),
              err->get_def_line()
          );
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

