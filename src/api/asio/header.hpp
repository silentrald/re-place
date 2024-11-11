/*============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-04-19
 *============================*/

// See:
// https://github.com/chriskohlhoff/asio/tree/master/asio/src/examples/cpp11/http/server

#ifndef API_ASIO_HEADER_HPP
#define API_ASIO_HEADER_HPP

#include "types.hpp"

namespace http::server {

struct header {
  string name;
  string value;
};

} // namespace http::server

#endif // HTTP_HEADER_HPP
