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

struct router {
  i32 method = -1;
  const char* path = nullptr;
  // Passed as the first parameter of the endpoint function call
  const void* data = nullptr;
  // First parameter contains the dependency injected classes
  void (*endpoint)(const void*, const request&, response&) noexcept = nullptr;
};

} // namespace http::server

#endif
