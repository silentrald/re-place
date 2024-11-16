#include "api/asio/server.hpp"
#include "api/auth.hpp"
#include "repo/pg.hpp"
#include "repo/user/pg.hpp"
#include "service/auth.hpp"
#include "types.hpp"
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
  service::Auth<repo::UserPg> auth_service{&user_repo};

  logger::info("Done Setting, trying to start server");

  try {
    // This can throw an error
    http::server::server server{};

    server.init("127.0.0.1", "5000");
    api::auth::setup(server, &auth_service);

    logger::info("Running: http://127.0.0.1:5000");
    server.run();
    logger::info("Exiting");
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
