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
#include <cstring>

namespace http::server {

struct response;
struct request;

struct dynamic_router_data {
  // Identifier for path
  string name{};
  // 0xFFFF means unset
  //  MSB tells whether it links to a dynamic or static
  //  The other bits tells the position
  u32 id_ptr = 0xFFFFFFFF;
};

struct static_router_data {
  // Key: used for routing path -> auth/login
  //  auth login
  // Value:
  //  MSB tells whether it links to a dynamic or static
  //  The other bits tells the position
  hash_map<string, u32> map{};
  i32 endpoint_pos = -1;
};

struct router_key {
  const c8* path = nullptr;
  i32 method = -1;

  // For comparison
  [[nodiscard]] i32
  operator()(const router_key& k1, const router_key& k2) const noexcept;
};

class request_handler {
public:
  request_handler(const request_handler&) noexcept = delete;
  request_handler& operator=(const request_handler&) noexcept = delete;

  request_handler() noexcept = default;
  request_handler(request_handler&& rhs) noexcept = default;
  request_handler& operator=(request_handler&& rhs) noexcept = default;
  ~request_handler() noexcept = default;

  // === Functions === //
  opt_err set_base_path(const string& base) noexcept;
  opt_err set_base_path(const char* base) noexcept;

  opt_err add_route(router&& route) noexcept;
  opt_err finalize() noexcept;

  void handle_request(request& req, response& res) noexcept;

private:
  string base{};

  vector<static_router_data> static_routes{};
  vector<dynamic_router_data> dynamic_routes{};
  vector<u32> endpoints{};
  vector<function<void(const request&, response&)>> callbacks{};

  map<router_key, function<void(const request&, response&)>, router_key>
      routers{};

  [[nodiscard]] exp_err<u32> create_static_route() noexcept;
  [[nodiscard]] exp_err<u32> create_dynamic_route() noexcept;

  [[nodiscard]] opt_err add_endpoint(static_router_data* sptr, u32 method, function<void(const request&, response&)>&& endpoint) noexcept;
  [[nodiscard]] exp_err<u32> create_endpoint(
      u32 method, function<void(const request&, response&)>&& endpoint
  ) noexcept;
  [[nodiscard]] opt_err append_endpoint(
      u32 method, function<void(const request&, response&)>&& endpoint
  ) noexcept;

  // === Debugging Purposes === //
  void print_routes() const noexcept;
};

} // namespace http::server

#endif // HTTP_REQUEST_HANDLER_HPP
