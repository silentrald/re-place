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
#include "mime_types.hpp"
#include "request.hpp"
#include "response.hpp"
#include "types.hpp"
#include "utils/logger/logger.hpp"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <utility>

#if 0
#include <fstream>
#include <sstream>
#endif

namespace http::server {

const c8 DYNAMIC_PREFIX = ':';

[[nodiscard]] bool is_dynamic(u32 pos) noexcept {
  return pos & 0x80000000;
}

[[nodiscard]] i32 get_pos(u32 pos) noexcept {
  return pos & 0x7FFFFFFF;
}

[[nodiscard]] bool is_new_endpoint(u32 method) noexcept {
  return method & 0x80000000;
}

[[nodiscard]]

[[nodiscard]] u8
get_method(u32 method) noexcept {
  return method;
}

i32 router_key::operator()(const router_key& k1, const router_key& k2)
    const noexcept {
  i32 diff = std::strcmp(k1.path, k2.path);
  return diff != 0 ? diff : k1.method - k2.method;
}

error_code request_handler::add_route(router&& route) noexcept {
  assert(route.path != nullptr);

  RP_TRY(this->routers.insert(
      {.path = route.path, .method = route.method}, std::move(route.endpoint)
  ));
  return error::OK;
}

expected<u32, error_code> request_handler::create_static_route() noexcept {
  RP_TRY(this->static_routes.push(static_router_data{}), ds::to_unexpected);
  return this->static_routes.get_size() - 1U;
}

expected<u32, error_code> request_handler::create_dynamic_route() noexcept {
  RP_TRY(this->dynamic_routes.push(dynamic_router_data{}), ds::to_unexpected);
  return 0x8000'0000 | (this->dynamic_routes.get_size() - 1U);
}

error_code request_handler::add_endpoint(
    static_router_data* sptr, u32 method,
    function<void(const request&, response&)>&& endpoint
) noexcept {
  if (sptr->endpoint_pos == -1) {
    sptr->endpoint_pos = RP_TRY_RETURN(
        this->create_endpoint(method, std::move(endpoint)), rp::to_error_code
    );
  } else {
    RP_TRY(this->append_endpoint(method, std::move(endpoint)));
  }
  return error::OK;
}

expected<u32, error_code> request_handler::create_endpoint(
    u32 method, function<void(const request&, response&)>&& endpoint
) noexcept {
  method |= 0x80000000;
  RP_TRY(this->endpoints.push(method), rp::to_unexpected);
  RP_TRY(this->callbacks.push(std::move(endpoint)), rp::to_unexpected);

  return this->endpoints.get_size() - 1;
}

error_code request_handler::append_endpoint(
    u32 method, function<void(const request&, response&)>&& endpoint
) noexcept {
  RP_TRY(this->endpoints.push(method));
  RP_TRY(this->callbacks.push(std::move(endpoint)));
  return error::OK;
}

// NOTE: For debugging
#if 0
void request_handler::print_routes() const noexcept {
  // TODO: Create a debug logging system for this
  std::cout << "Static Routes\n";
  for (i32 i = 0; i < this->static_routes.size(); ++i) {
    auto& sr = this->static_routes[i];
    std::cout << i << ": " << std::hex << sr.endpoint_pos << std::dec << '\n';
    for (auto it = sr.map.begin(); it != sr.map.end(); ++it) {
      std::cout << "> " << it.key() << ' ' << std::hex << it.value() << std::dec
                << '\n';
    }
  }

  std::cout << "Dynamic Routes\n";
  for (i32 i = 0; i < this->dynamic_routes.size(); ++i) {
    const auto& dr = this->dynamic_routes[i];
    std::cout << i << ": " << dr.name.c_str() << ' ' << std::hex << dr.id_ptr
              << std::dec << '\n';
  }
}
#endif

// TODO: Can be refactored if needed
error_code request_handler::finalize() noexcept {
  // Id 0 is the start
  RP_TRY(this->static_routes.push(static_router_data{}));

  i32 start = 0;
  u32 id = 0U;
  u32 prev_id = 0U;
  string path{};
  string substr{};

  static_router_data* sptr = this->static_routes.get_data();
  dynamic_router_data* dptr = nullptr;

  for (auto it = this->routers.begin(); it != this->routers.end(); ++it) {
    RP_TRY(path.copy(it.key().path));

    if (path[path.get_size() - 1] == '/') {
      static_cast<void>(path.pop());
    }

    sptr = this->static_routes.get_data();
    prev_id = 0U;
    start = path[0] == '/' ? 1 : 0;
    for (i32 end = start; end <= path.get_size(); ++end) {
      if (path[end] != '/' && end < path.get_size()) {
        continue;
      }

      // TODO: Separate this to a function
      // Get the substring
      substr = RP_TRY_RETURN(path.substring(start, end), rp::to_error_code);
      start = end + 1;

      if (dptr) {
        // Go back to static
        if (dptr->id_ptr != 0xFFFFFFFF) {
          prev_id = dptr->id_ptr;
          if (is_dynamic(prev_id)) {
            dptr = &this->dynamic_routes[get_pos(prev_id)];
          } else {
            sptr = &this->static_routes[prev_id];
            dptr = nullptr;
          }
          continue;
        }

        id = RP_TRY_RETURN(this->create_static_route(), rp::to_error_code);
        dptr->id_ptr = id;

        if (dptr->name.is_empty()) {
          substr = RP_TRY_RETURN(substr.substring(1), rp::to_error_code);
          dptr->name = std::move(substr);
        }

        if (start < path.get_size() && path[start] == DYNAMIC_PREFIX) {
          // Append another dynamic route
          id = RP_TRY_RETURN(this->create_dynamic_route(), rp::to_error_code);
          dptr = &this->dynamic_routes[get_pos(prev_id)]; // Got invalidated
          prev_id = id;
          dptr->id_ptr = id;
          dptr = &this->dynamic_routes[get_pos(id)];
        } else {
          prev_id = dptr->id_ptr;
          sptr = &this->static_routes[prev_id];
          dptr = nullptr;
        }

        continue;
      }

      if (sptr->map.contains(substr)) {
        id = *sptr->map[substr];
      } else if (start < path.get_size() && path[start] == DYNAMIC_PREFIX) {
        id = RP_TRY_RETURN(this->create_dynamic_route(), rp::to_error_code);
        RP_TRY(sptr->map.insert(std::move(substr), id));
      } else {
        id = RP_TRY_RETURN(this->create_static_route(), rp::to_error_code);
        // sptr gets invalidated when create static route was called
        sptr = &this->static_routes[prev_id];
        RP_TRY(sptr->map.insert(std::move(substr), id));
      }

      if (is_dynamic(id)) {
        dptr = &this->dynamic_routes[get_pos(id)];
      } else {
        sptr = &this->static_routes[id];
      }
      prev_id = id;
    }

    auto& route = it.value();
    if (dptr) {
      id = RP_TRY_RETURN(this->create_static_route(), rp::to_error_code);
      dptr->id_ptr = id;
      sptr = &this->static_routes[id];
    }

    RP_TRY(this->add_endpoint(sptr, it.key().method, std::move(it.value())));
  }

  this->routers.clear();

  // For debugging
  /* this->print_routes(); */

  return error::OK;
}

void request_handler::handle_request(request& req, response& res) noexcept {
  auto* sroute = this->static_routes.get_data();
  dynamic_router_data* droute = nullptr;

  if (req.uri == "/") {
    if (sroute->endpoint_pos == -1) {
      res = response::stock_response(response::not_found);
    } else {
      this->callbacks[sroute->endpoint_pos](req, res);
    }
    return;
  }

  // Find the path here
  i32 size = req.uri.get_size() - (req.uri[req.uri.get_size() - 1] == '/');
  i32 start = 1;
  // TODO: Do not do a substring, do strncmp
  expected<string, error_code> exp_substr{};
  u32* ptr = nullptr;
  for (i32 end = start; end <= size; ++end) {
    if (req.uri[end] != '/' && end < size) {
      continue;
    }

    exp_substr = req.uri.substring(start, end);
    if (!exp_substr) {
      res = response::stock_response(response::internal_server_error);
      logger::error("substr in request_handler no memory");
      return;
    }

    if (droute) {
      path param{};
      auto err = param.key.copy(droute->name);
      if (err) {
        res = response::stock_response(response::internal_server_error);
        logger::error("substr in request_handler no memory");
        return;
      }

      param.value = std::move(exp_substr.value());
      err = req.path_params.push(std::move(param));
      if (err) {
        res = response::stock_response(response::internal_server_error);
        logger::error("substr in request_handler no memory");
        return;
      }

      if (is_dynamic(droute->id_ptr)) {
        droute = &this->dynamic_routes[get_pos(droute->id_ptr)];
      } else {
        sroute = &this->static_routes[droute->id_ptr];
        droute = nullptr;
      }
      start = end + 1;
      continue;
    }

    ptr = sroute->map[*exp_substr];
    if (ptr == nullptr) {
      res = response::stock_response(response::not_found);
      return;
    }

    if (is_dynamic(*ptr)) {
      droute = &this->dynamic_routes[get_pos(*ptr)];
    } else {
      sroute = &this->static_routes[*ptr];
    }
    start = end + 1;
  }

  if (sroute->endpoint_pos == -1) {
    res = response::stock_response(response::not_found);
    return;
  }

  // Find the appropriate endpoint
  u32 endpoint = this->endpoints[sroute->endpoint_pos];
  if (get_method(endpoint) == req.method) {
    this->callbacks[sroute->endpoint_pos](req, res);
    return;
  }

  for (start = sroute->endpoint_pos + 1;
       start < this->endpoints.get_size() && !is_new_endpoint(start); ++start) {
    if (get_method(this->endpoints[start] == req.method)) {
      this->callbacks[start](req, res);
      return;
    }
  }

  res = response::stock_response(response::not_found);
}

} // namespace http::server
