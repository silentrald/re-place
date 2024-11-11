/*============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-04-19
 *============================*/

// See:
// https://github.com/chriskohlhoff/asio/tree/master/asio/src/examples/cpp11/http/server

#ifndef API_ASIO_CONNECTION_HPP
#define API_ASIO_CONNECTION_HPP

#include "asio/ip/tcp.hpp"
#include "llhttp.h"
#include "request.hpp"
#include "request_handler.hpp"
#include "response.hpp"
#include "types.hpp"
#include <memory>

namespace http::server {

class connection_manager;

struct req_data {
  string tmp;
  request* req = nullptr;
};

// TODO: Think of a way to not inherit enabled_shared_from_this
/// Represents a single connection from a client.
class connection : public std::enable_shared_from_this<connection> {
public:
  connection(const connection&) = delete;
  connection& operator=(const connection&) = delete;

  explicit connection(
      asio::ip::tcp::socket socket, connection_manager& manager,
      request_handler& handler
  );

  connection(connection&& rhs) noexcept = default;
  connection& operator=(connection&& rhs) noexcept = delete;

  ~connection() noexcept = default;

  void start();
  void stop();

private:
  asio::ip::tcp::socket socket;

  connection_manager* conn_manager;
  request_handler* req_handler;
  array<char, 8192> buffer{};

  llhttp_t parser{};
  llhttp_settings_t settings{};

  req_data data{};
  request req{};
  response res{};

  void do_read() noexcept;
  void do_write() noexcept;
};

using connection_ptr = std::shared_ptr<connection>;

} // namespace http::server

#endif // HTTP_CONNECTION_HPP
