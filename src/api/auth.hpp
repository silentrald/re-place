
#ifndef API_AUTH_HPP
#define API_AUTH_HPP

#include "api/asio/response.hpp"
#include "api/asio/request.hpp"
#include "api/asio/router.hpp"
#include "ds/types.hpp"
#include "llhttp.h"
#include "repo/pg/user.hpp"
#include "use-case/auth/login.hpp"

namespace api {

http::server::router get_auth_login();

} // namespace api

#endif

