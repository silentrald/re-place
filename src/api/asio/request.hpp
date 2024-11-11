/*============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-04-19
 *============================*/

// See:
// https://github.com/chriskohlhoff/asio/tree/master/asio/src/examples/cpp11/http/server

#ifndef API_ASIO_REQUEST_HPP
#define API_ASIO_REQUEST_HPP

#include "header.hpp"
#include "rapidjson/document.h"
#include "types.hpp"

using json = rapidjson::Document;

namespace http::server {

struct path {
  string key;
  string value;
};

class request_handler;

struct request {
public:
  friend request_handler;

  string uri;
  string body;
  vector<header> headers;

private:
  // TODO: Use union
  json body_params;
  vector<path> path_params{};

public:
  u8 method = 0U;
  u8 http_version_major = 0U;
  u8 http_version_minor = 0U;

  void finish() {
    for (const auto& h : this->headers) {
      if (h.name == "content-type" && h.value == "application/json" &&
          !this->body.is_empty()) {
        this->body_params.Parse(this->body.c_str());
        break;
      }
    }
  }

  [[nodiscard]] const char* get_body_parameter(const char* param) const {
    return this->body_params[param].GetString();
  }

  [[nodiscard]] const char* get_path_parameter(const char* param) const {
    for (i32 i = 0; i < this->path_params.get_size(); ++i) {
      const auto& p = this->path_params[i];
      if (p.key == param) {
        return p.value.c_str();
      }
    }
    return nullptr;
  }
};

} // namespace http::server

#endif // HTTP_REQUEST_HPP
