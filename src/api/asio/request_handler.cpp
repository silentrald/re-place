/*============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-04-19
 *============================*/

// See:
// https://github.com/chriskohlhoff/asio/tree/master/asio/src/examples/cpp11/http/server

#include "request_handler.hpp"
#include "api/asio/router.hpp"
#include "config/types.hpp"
#include "ds/macro.hpp"
#include "mime_types.hpp"
#include "request.hpp"
#include "response.hpp"
#include <cstdio>
#include <fstream>
#include <sstream>
#include <utility>

namespace http::server {

opt_err request_handler::add_route(router&& route) noexcept {
  if (this->routers.push_back(std::forward<router&&>(route))) {
    return error{"Could not add route", def_err_vals};
  }
  return null;
}

void request_handler::handle_request(
    const request& req, response& rep
) noexcept {
  printf("GET %s\n", req.uri.c_str()); // TODO: Logger

  // NOTE: Simple implementation, no dynamic pathing
  for (const auto& r : this->routers) {
    if (req.uri == r.path) {
      r.endpoint(req, rep);
      return;
    }
  }

  // TODO: Log here
  rep = response::stock_response(response::not_found);
}

} // namespace http::server
