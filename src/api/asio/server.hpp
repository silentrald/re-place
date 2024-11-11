/*============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-04-19
 *============================*/

// See:
// https://github.com/chriskohlhoff/asio/tree/master/asio/src/examples/cpp11/http/server

#ifndef API_ASIO_SERVER_HPP
#define API_ASIO_SERVER_HPP

#include "connection.hpp"
#include "connection_manager.hpp"
#include "request_handler.hpp"
#include "router.hpp"
#include "types.hpp"
#include <asio.hpp>

namespace http::server {

/// The top-level class of the HTTP server.
class server {
public:
  server() noexcept = default;

  server(const server&) noexcept = delete;
  server& operator=(const server&) noexcept = delete;

  server(server&& rhs) noexcept = delete;
  server& operator=(server&& rhs) noexcept = delete;

  ~server() noexcept = default;

  // === Constructors === //

  error_code init(const char* host, const char* port) noexcept;

  // === Functions === //

  void add_route(router&& route) noexcept;

  error_code run() noexcept;

private:
  // TODO: Check if this can be separated so that I don't have to initialize it
  asio::io_context io_cxt{1};
  asio::signal_set signals{this->io_cxt};
  asio::ip::tcp::acceptor acceptor{this->io_cxt};
  connection_manager conn_manager;
  request_handler req_handler;

  void do_accept() noexcept;
  void do_await_stop() noexcept;
};

} // namespace http::server

#endif // HTTP_SERVER_HPP
