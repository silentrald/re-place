#include "api/asio/server.hpp"
#include "api/auth.hpp"
#include "config/types.hpp"
#include "ds-error/error.hpp"
#include "ds/macro.hpp"
#include "ds/types.hpp"
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

int main(int argc, char* argv[]) {
  // Declare variables here
  repo::PgManager repo{};
  repo.init(USER, PASS, DB, HOST, PORT);
  repo::UserPg user_repo{&repo};
  use_case::auth::Login<repo::UserPg> login_uc{&user_repo};

  try {
    // This can throw an error
    http::server::server srvr{};

    srvr.init("127.0.0.1", "5000");

    srvr.add_route(api::get_auth_login(&login_uc));

    printf("Running: http://127.0.0.1:5000\n");
    srvr.run();
  } catch (std::exception& e) {
    std::cerr << "exception: " << e.what() << "\n";
  }
}

