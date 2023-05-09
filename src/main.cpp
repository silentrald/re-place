#include "api/asio/router.hpp"
#include "api/asio/server.hpp"
#include "api/auth.hpp"
#include "config/logger.hpp"
#include "config/types.hpp"
#include "ds/macro.hpp"
#include "entity/user.hpp"
#include "llhttp.h"
#include "repo/pg/user.hpp"
#include "use-case/auth/login.hpp"
#include <cstdio>
#include <cstring>
#include <iostream>

const char* const USER = "sample_user";
const char* const PASS = "password";
const char* const DB = "sample_db";
const char* const HOST = "127.0.0.1";
const char* const PORT = "5432";

opt_err main_wrapper() noexcept {
  // Declare variables here
  repo::PgManager repo{};
  try_opt(repo.init(USER, PASS, DB, HOST, PORT));

  repo::UserPg user_repo{&repo};
  use_case::auth::Login<repo::UserPg> login_uc{&user_repo};

  api::PostLogin<repo::UserPg> post_login{&login_uc};

  try {
    // This can throw an error
    http::server::server srvr{};

    srvr.init("127.0.0.1", "5000");

    srvr.add_route(http::server::router{
        .method = HTTP_POST, .path = "/api/auth/login", .endpoint = post_login}
    );

    logger::info("Running: http://127.0.0.1:5000");
    srvr.run();
  } catch (std::exception& e) {
    std::cerr << "exception: " << e.what() << "\n";
  }

  return null;
}

int main(int argc, char* argv[]) {
  auto err = main_wrapper();
  if (err) {
    logger::fatal(*err);
    return -1;
  }
}

