#include "api/asio/router.hpp"
#include "api/asio/server.hpp"
#include "api/auth.hpp"
#include "cache/redis.hpp"
#include "cache/redis/session.hpp"
#include "config/logger.hpp"
#include "config/types.hpp"
#include "ds/macro.hpp"
#include "llhttp.h"
#include "repo/pg/user.hpp"
#include "use-case/auth/login.hpp"
#include <cstdio>
#include <cstring>
#include <iostream>

const char* const USER = "username";
const char* const PASS = "password";
const char* const DB = "replace";
const char* const HOST = "127.0.0.1";
const char* const PORT = "5432";

opt_err main_wrapper() noexcept {
  logger::info("Setting Up");

  repo::PgManager repo{};
  try_opt(repo.init(USER, PASS, DB, HOST, PORT));

  cache::RedisManager redis{};
  try_opt(redis.init("127.0.0.1", 6379));

  repo::UserPg user_repo{};
  try_opt(user_repo.init(&repo));

  cache::RedisSession session{};
  try_opt(session.init(&redis));

  use_case::auth::Login<repo::UserPg, cache::RedisSession> login_uc{};
  login_uc.init(&user_repo, &session);

  api::PostLogin<repo::UserPg, cache::RedisSession> post_login{};
  try_opt(post_login.init(&login_uc));

  logger::info("Done Setting, trying to start server");

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

