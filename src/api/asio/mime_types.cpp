/*============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-04-19
 *============================*/

// See:
// https://github.com/chriskohlhoff/asio/tree/master/asio/src/examples/cpp11/http/server

#include "mime_types.hpp"
#include "types.hpp"
#include <cstring>

namespace http::server::mime_types {

struct mapping { // NOLINT
  const char* extension;
  const char* mime_type;
} mappings[] = {
    {"gif", "image/gif"},
    {"htm", "text/html"},
    {"html", "text/html"},
    {"jpg", "image/jpeg"},
    {"png", "image/png"}};

const char* extension_to_type(const string& extension) {
  for (mapping m : mappings) {
    if (m.extension == extension) {
      return m.mime_type;
    }
  }

  return "text/plain";
}

const char* extension_to_type(const char* extension) {
  for (mapping m : mappings) {
    if (strcmp(m.extension, extension) == 0) {
      return m.mime_type;
    }
  }

  return "text/plain";
}

} // namespace http::server::mime_types
