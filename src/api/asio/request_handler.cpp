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
#include "config/logger.hpp"
#include "config/types.hpp"
#include "ds/macro.hpp"
#include "mime_types.hpp"
#include "request.hpp"
#include "response.hpp"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <sstream>
#include <utility>

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

opt_err request_handler::add_route(router&& route) noexcept {
  if (route.path == nullptr) {
    return error{"Path cannot be null", def_err_vals};
  }

  try_opt(this->routers.insert(
      {.path = route.path, .method = route.method}, std::move(route.endpoint)
  ));
  return null;
}

exp_err<u32> request_handler::create_static_route() noexcept {
  try_opt_unexp(this->static_routes.push_back(static_router_data{}));
  return this->static_routes.size() - 1U;
}

exp_err<u32> request_handler::create_dynamic_route() noexcept {
  try_opt_unexp(this->dynamic_routes.push_back(dynamic_router_data{}));
  return 0x80000000 | (this->dynamic_routes.size() - 1U);
}

opt_err request_handler::add_endpoint(
    static_router_data* sptr, u32 method,
    function<void(const request&, response&)>&& endpoint
) noexcept {
  if (sptr->endpoint_pos == -1) {
    sptr->endpoint_pos =
        try_exp_err(this->create_endpoint(method, std::move(endpoint)));
  } else {
    try_opt(this->append_endpoint(method, std::move(endpoint)));
  }
  return null;
}

exp_err<u32> request_handler::create_endpoint(
    u32 method, function<void(const request&, response&)>&& endpoint
) noexcept {
  method |= 0x80000000;
  try_opt_unexp(this->endpoints.push_back(method));
  try_opt_unexp(this->callbacks.push_back(std::move(endpoint)));

  return this->endpoints.size() - 1;
}

opt_err request_handler::append_endpoint(
    u32 method, function<void(const request&, response&)>&& endpoint
) noexcept {
  try_opt(this->endpoints.push_back(method));
  try_opt(this->callbacks.push_back(std::move(endpoint)));
  return null;
}

/* void request_handler::print_routes() const noexcept { */
/*   // TODO: Create a debug logging system for this */
/*   std::cout << "Static Routes\n"; */
/*   for (i32 i = 0; i < this->static_routes.size(); ++i) { */
/*     auto& sr = this->static_routes[i]; */
/*     std::cout << i << ": " << std::hex << sr.endpoint_pos << std::dec << '\n'; */
/*     for (auto it = sr.map.begin(); it != sr.map.end(); ++it) { */
/*       std::cout << "> " << it.key() << ' ' << std::hex << it.value() << std::dec */
/*                 << '\n'; */
/*     } */
/*   } */

/*   std::cout << "Dynamic Routes\n"; */
/*   for (i32 i = 0; i < this->dynamic_routes.size(); ++i) { */
/*     const auto& dr = this->dynamic_routes[i]; */
/*     std::cout << i << ": " << dr.name.c_str() << ' ' << std::hex << dr.id_ptr */
/*               << std::dec << '\n'; */
/*   } */
/* } */

// TODO: Can be refactored if needed
opt_err request_handler::finalize() noexcept {
  // Id 0 is the start
  try_opt(this->static_routes.push_back(static_router_data{}));

  i32 start = 0;
  u32 id = 0U;
  u32 prev_id = 0U;
  string path{};
  string substr{};

  static_router_data* sptr = this->static_routes.data();
  dynamic_router_data* dptr = nullptr;

  for (auto it = this->routers.begin(); it != this->routers.end(); ++it) {
    try_opt(path.copy(it.key().path));

    if (path[path.size() - 1] == '/') {
      // TODO: Create an pop_back_unsafe
      static_cast<void>(path.pop_back());
    }

    sptr = this->static_routes.data();
    prev_id = 0U;
    start = path[0] == '/' ? 1 : 0;
    for (i32 end = start; end <= path.size(); ++end) {
      if (path[end] != '/' && end < path.size()) {
        continue;
      }

      // TODO: Separate this to a function
      // Get the substring
      substr = try_exp_err(path.substr(start, end));
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

        id = try_exp_err(this->create_static_route());
        dptr->id_ptr = id;

        if (dptr->name.is_empty()) {
          substr = try_exp_err(substr.substr(1));
          dptr->name = std::move(substr);
        }

        if (start < path.size() && path[start] == DYNAMIC_PREFIX) {
          // Append another dynamic route
          id = try_exp_err(this->create_dynamic_route());
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
      } else if (start < path.size() && path[start] == DYNAMIC_PREFIX) {
        id = try_exp_err(this->create_dynamic_route());
        try_opt(sptr->map.insert(std::move(substr), id));
      } else {
        id = try_exp_err(this->create_static_route());
        // sptr gets invalidated when create static route was called
        sptr = &this->static_routes[prev_id];
        try_opt(sptr->map.insert(std::move(substr), id));
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
      id = try_exp_err(this->create_static_route());
      dptr->id_ptr = id;
      sptr = &this->static_routes[id];
    }

    try_opt(this->add_endpoint(sptr, it.key().method, std::move(it.value())));
  }

  this->routers.clear();

  // For debugging
  /* this->print_routes(); */

  return null;
}

void request_handler::handle_request(request& req, response& res) noexcept {
  auto* sroute = this->static_routes.data();
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
  i32 size = req.uri.size() - (req.uri[req.uri.size() - 1] == '/');
  i32 start = 1;
  exp_err<string> exp_substr{};
  u32* ptr = nullptr;
  for (i32 end = start; end <= size; ++end) {
    if (req.uri[end] != '/' && end < size) {
      continue;
    }

    exp_substr = req.uri.substr(start, end);
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
      err = req.path_params.push_back(std::move(param));
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
       start < this->endpoints.size() && !is_new_endpoint(start); ++start) {
    if (get_method(this->endpoints[start] == req.method)) {
      this->callbacks[start](req, res);
      return;
    }
  }

  res = response::stock_response(response::not_found);
}

} // namespace http::server
