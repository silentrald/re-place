#include "api/asio/router.hpp"
#include "api/asio/server.hpp"
#include "api/auth.hpp"
#include "llhttp.h"
#include "repo/pg.hpp"
#include "repo/user/pg.hpp"
#include "types.hpp"
#include "use-case/auth/login.hpp"
#include "use-case/auth/register.hpp"
#include "utils/logger/logger.hpp"
#include <cstdio>
#include <cstring>

const char* const USER = "username";
const char* const PASS = "password";
const char* const DB = "replace";
const char* const HOST = "127.0.0.1";
const char* const PORT = "5432";

error_code main_wrapper() noexcept {
  logger::info("Setting Up");

  repo::PgManager repo{};
  RP_TRY(repo.init(USER, PASS, DB, HOST, PORT));

  repo::UserPg user_repo{&repo};

  use_case::auth::Login<repo::UserPg> login_use_case{&user_repo};
  api::PostLogin<repo::UserPg> post_login{&login_use_case};
  use_case::auth::Register<repo::UserPg> register_use_case{&user_repo};
  api::PostRegister<repo::UserPg> post_register{&register_use_case};

  logger::info("Done Setting, trying to start server");

  try {
    // This can throw an error
    http::server::server srvr{};

    srvr.init("127.0.0.1", "5000");

    srvr.add_route(http::server::router{
        .method = HTTP_POST, .path = "/api/auth/login", .endpoint = post_login
    });
    srvr.add_route(http::server::router{
        .method = HTTP_POST,
        .path = "/api/auth/register",
        .endpoint = post_register
    });

    logger::info("Running: http://127.0.0.1:5000");
    srvr.run();
  } catch (std::exception& e) {
    logger::error("Exception: %s", e.what());
  }

  return error::OK;
}

int main(int argc, char* argv[]) {
  auto err = main_wrapper();
  if (err) {
    logger::fatal("Got error code %u", err);
    return -1;
  }
}
