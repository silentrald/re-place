/*============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-04-19
 *============================*/

// See:
// https://github.com/chriskohlhoff/asio/tree/master/asio/src/examples/cpp11/http/server

#include "connection_manager.hpp"

namespace http::server {

// NOLINTNEXTLINE
void connection_manager::start(connection_ptr c) noexcept {
  this->connections.insert(c);
  c->start();
}

// NOLINTNEXTLINE
void connection_manager::stop(connection_ptr c) noexcept {
  this->connections.erase(c);
  c->stop();
}

void connection_manager::stop_all() noexcept {
  for (const auto& c : this->connections)
    c->stop();
  this->connections.clear();
}

} // namespace http::server
