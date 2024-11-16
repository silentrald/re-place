#include "api/asio/server.hpp"
#include "api/auth.hpp"
#include "repo/pg.hpp"
#include "repo/user/pg.hpp"
#include "service/auth.hpp"
#include "types.hpp"
#include "utils/logger/logger.hpp"
#include <cstdio>
#include <cstring>

const c8* const DB_USER = "username";
const c8* const DB_PASS = "password";
const c8* const DB_NAME = "replace";
const c8* const DB_HOST = "127.0.0.1";
const c8* const DB_PORT = "5432";

const c8* const SERVER_HOST = "127.0.0.1";
const c8* const SERVER_PORT = "5000";

error_code main_wrapper() noexcept {
  logger::info("Initializing server");

  repo::PgManager repo{};
  RP_TRY(repo.init(DB_USER, DB_PASS, DB_NAME, DB_HOST, DB_PORT));

  repo::UserPg user_repo{&repo};
  service::Auth<repo::UserPg> auth_service{&user_repo};

  logger::info("Done initialization, trying to start server");

  try {
    // This can throw an error
    http::server::server server{};

    server.init(SERVER_HOST, SERVER_PORT);
    api::auth::setup(server, &auth_service);

    logger::info("Server running at: http://%s:%s", SERVER_HOST, SERVER_PORT);
    server.run();

    printf("\n"); // For the signal character
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
