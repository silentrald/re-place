/*============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-04-19
 *============================*/

// See:
// https://github.com/chriskohlhoff/asio/tree/master/asio/src/examples/cpp11/http/server

#ifndef API_ASIO_CONNECTION_MANAGER_HPP
#define API_ASIO_CONNECTION_MANAGER_HPP

#include "types.hpp"
#include "connection.hpp"

namespace http::server {

class connection_manager {
public:
  connection_manager(const connection_manager&) = delete;
  connection_manager& operator=(const connection_manager&) = delete;

  connection_manager() noexcept = default;

  connection_manager(connection_manager&& rhs) noexcept = default;
  connection_manager& operator=(connection_manager&& rhs) noexcept = default;

  ~connection_manager() noexcept = default;

  void start(connection_ptr c) noexcept;
  void stop(connection_ptr c) noexcept;
  void stop_all() noexcept;

private:
  set<connection_ptr> connections{};
};

} // namespace http::server

#endif // HTTP_CONNECTION_MANAGER_HPP

