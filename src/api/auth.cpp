
#include "./auth.hpp"
#include "api/asio/router.hpp"

namespace api {

http::server::router get_auth_login() {
  return http::server::router{
      .method = HTTP_GET,
      .path = "/api/login",
      // TODO: Edit this to pass scoped variables
      .endpoint = [](const http::server::request& req,
                     http::server::reply& rep) {
        repo::UserPg user_repo{};
        use_case::auth::Login<repo::UserPg> login_uc{&user_repo};

        // TODO: Json parser
        types::string username{};
        ds::err_code err = username.copy("username");
        types::string password{};
        err = password.copy("password");

        login_uc.execute(username, password);
      }};
}

} // namespace api

