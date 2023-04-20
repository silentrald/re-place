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

#include "config/types.hpp"
#include "ds/macro.hpp"
#include "ds/types.hpp"
#include "header.hpp"
#include "rapidjson/document.h"
#include <iostream>

using json = rapidjson::Document;

namespace http::server {

struct request {
public:
  types::string uri{};
  types::string body{};
  types::vector<header> headers{};

private:
  // TODO: Use union
  json parameters{};

public:
  types::u8 method = 0U;
  types::u8 http_version_major = 0U;
  types::u8 http_version_minor = 0U;

  void finish() {
    for (const auto& h : this->headers) {
      if (h.name == "content-type" && h.value == "application/json" &&
          !this->body.is_empty()) {
        this->parameters.Parse(this->body.c_str());
        break;
      }
    }
  }

  types::exp_err<types::string> get_parameter(const char* param) const {
    types::string str{};
    auto ec = str.copy(
        this->parameters[param].GetString(),
        this->parameters[param].GetStringLength()
    );
    if (ec != types::SUCCESS) {
      return types::unexp_err{
          types::error{"Could not get param", def_err_vals}};
    }
    return str;
  }
};

} // namespace http::server

#endif // HTTP_REQUEST_HPP
