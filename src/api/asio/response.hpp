/*============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-04-19
 *============================*/

// See:
// https://github.com/chriskohlhoff/asio/tree/master/asio/src/examples/cpp11/http/server

#ifndef API_ASIO_RESPONSE_HPP
#define API_ASIO_RESPONSE_HPP

#include "asio.hpp"
#include "config/types.hpp"
#include "header.hpp"

namespace http::server {

struct response {
  enum status_type {
    ok = 200,
    created = 201,
    accepted = 202,
    no_content = 204,
    multiple_choices = 300,
    moved_permanently = 301,
    moved_temporarily = 302,
    not_modified = 304,
    bad_request = 400,
    unauthorized = 401,
    forbidden = 403,
    not_found = 404,
    internal_server_error = 500,
    not_implemented = 501,
    bad_gateway = 502,
    service_unavailable = 503
  } status = status_type::ok;

  types::vector<header> headers;
  types::string content;

  std::vector<asio::const_buffer> to_buffers();

  static response stock_response(status_type status);
};

} // namespace http::server

#endif // HTTP_REPLY_HPP
