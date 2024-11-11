/*============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-04-19
 *============================*/

#ifndef API_ASIO_ROUTER_HPP
#define API_ASIO_ROUTER_HPP

#include "./request.hpp"
#include "./response.hpp"
#include "types.hpp"

namespace http::server {

struct router { // NOLINT
  i32 method = -1;
  const char* path = nullptr;
  function<void(const request&, response&)> endpoint;
};

} // namespace http::server

#endif
