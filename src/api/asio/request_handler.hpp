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
#include "types.hpp"
#include <cassert>
#include <cstring>

namespace http::server {

struct response;
struct request;

struct dynamic_router_data {
  // Identifier for path
  string name;
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

struct router_callback {
  const void* data = nullptr;
  void (*endpoint)(const void*, const request&, response&) noexcept = nullptr;

  error_code copy(const router_callback& other) noexcept {
    this->data = other.data;
    this->endpoint = other.endpoint;
    return error::OK;
  }

  inline void execute(const request& req, response& res) const noexcept {
    assert(this->endpoint != nullptr);
    this->endpoint(data, req, res);
  }
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
  error_code set_base_path(const string& base) noexcept;
  error_code set_base_path(const char* base) noexcept;

  error_code add_route(router route) noexcept;
  error_code finalize() noexcept;

  void handle_request(request& req, response& res) noexcept;

private:
  string base;

  vector<static_router_data> static_routes{};
  vector<dynamic_router_data> dynamic_routes{};
  vector<u32> endpoints{};
  vector<router_callback> callbacks{};

  map<router_key, router_callback, router_key> routers{};

  [[nodiscard]] expected<u32, error_code> create_static_route() noexcept;
  [[nodiscard]] expected<u32, error_code> create_dynamic_route() noexcept;

  [[nodiscard]] error_code add_endpoint(
      static_router_data* sptr, u32 method, const router_callback& callback
  ) noexcept;
  [[nodiscard]] expected<u32, error_code>
  create_endpoint(u32 method, const router_callback& callback) noexcept;
  [[nodiscard]] error_code
  append_endpoint(u32 method, const router_callback& callback) noexcept;
};

} // namespace http::server

#endif // HTTP_REQUEST_HANDLER_HPP
