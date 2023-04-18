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
#include <cstring>
#include <iostream>

int main(int argc, char* argv[]) {
  try {
    // Initialise the server.
    http::server::server s("127.0.0.1", "5000");

    s.add_route(api::get_auth_login());

    // Run the server until stopped.
    s.run();
  } catch (std::exception& e) {
    std::cerr << "exception: " << e.what() << "\n";
  }
}

