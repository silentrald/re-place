

#ifndef HTTP_ROUTER_HPP
#define HTTP_ROUTER_HPP

#include "./reply.hpp"
#include "./request.hpp"
#include <string>

namespace http::server {

struct router {
  int method = -1;
  std::string path{};
  void (*endpoint)(const request& req, reply& rep);
};

} // namespace http::server

#endif

