//
// request_handler.cpp
// ~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2023 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include "request_handler.hpp"
#include "api/asio/router.hpp"
#include "mime_types.hpp"
#include "reply.hpp"
#include "request.hpp"
#include <fstream>
#include <sstream>
#include <string>

namespace http::server {

void request_handler::add_route(const router& route) {
  this->routers.emplace_back(route);
}

void request_handler::add_route(router&& router) {
  this->routers.emplace_back(std::move(router));
}

void request_handler::handle_request(const request& req, reply& rep) {
  // NOTE: Simple implementation, no dynamic pathing
  for (const auto& r : this->routers) {
    if (req.uri == r.path) {
      r.endpoint(req, rep);
      return;
    }
  }

  rep = reply::stock_reply(reply::not_found);
}

bool request_handler::url_decode(const std::string& in, std::string& out) {
  out.clear();
  out.reserve(in.size());
  for (std::size_t i = 0; i < in.size(); ++i) {
    if (in[i] == '%') {
      if (i + 3 <= in.size()) {
        int value = 0;
        std::istringstream is(in.substr(i + 1, 2));
        if (is >> std::hex >> value) {
          out += static_cast<char>(value);
          i += 2;
        } else {
          return false;
        }
      } else {
        return false;
      }
    } else if (in[i] == '+') {
      out += ' ';
    } else {
      out += in[i];
    }
  }
  return true;
}

} // namespace http::server
