//
// request_handler.hpp
// ~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2023 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef HTTP_REQUEST_HANDLER_HPP
#define HTTP_REQUEST_HANDLER_HPP

#include "./router.hpp"
#include <string>
#include <vector>

namespace http::server {

struct reply;
struct request;

/// The common handler for all incoming requests.
class request_handler {
public:
  request_handler(const request_handler&) = delete;
  request_handler& operator=(const request_handler&) = delete;

  /// Construct with a directory containing files to be served.
  request_handler() noexcept = default;

  void add_route(const router& route);
  void add_route(router&& route);

  /// Handle a request and produce a reply.
  void handle_request(const request& req, reply& rep);

private:
  /// Perform URL-decoding on a string. Returns false if the encoding was
  /// invalid.
  static bool url_decode(const std::string& in, std::string& out);

  std::vector<router> routers{};
};

} // namespace http::server

#endif // HTTP_REQUEST_HANDLER_HPP
