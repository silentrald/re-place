/*============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-04-19
 *============================*/

// See:
// https://github.com/chriskohlhoff/asio/tree/master/asio/src/examples/cpp11/http/server

#ifndef API_ASIO_MIME_TYPES_HPP
#define API_ASIO_MIME_TYPES_HPP

#include "config/types.hpp"

namespace http::server::mime_types {

const char* extension_to_type(const types::string& extension);
const char* extension_to_type(const char* extension);

} // namespace http::server::mime_types

#endif // HTTP_MIME_TYPES_HPP
