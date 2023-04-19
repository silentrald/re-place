/*============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-04-19
 *============================*/

// See:
// https://github.com/chriskohlhoff/asio/tree/master/asio/src/examples/cpp11/http/server

#ifndef API_ASIO_REQUEST_HANDLER_HPP
#define API_ASIO_REQUEST_HANDLER_HPP

#include "./router.hpp"
#include "config/types.hpp"

namespace http::server {

struct response;
struct request;

class request_handler {
private:
  types::string base{};
  types::vector<router> routers{};

public:
  request_handler(const request_handler&) noexcept = delete;
  request_handler& operator=(const request_handler&) noexcept = delete;

  request_handler() noexcept = default;
  request_handler(request_handler&& rhs) noexcept = default;
  request_handler& operator=(request_handler&& rhs) noexcept = default;
  ~request_handler() noexcept = default;

  // === Functions === //
  types::opt_err set_base_path(const types::string& base) noexcept;
  types::opt_err set_base_path(const char* base) noexcept;

  types::opt_err add_route(router&& route) noexcept;

  void handle_request(const request& req, response& rep) noexcept;
};

} // namespace http::server

#endif // HTTP_REQUEST_HANDLER_HPP
