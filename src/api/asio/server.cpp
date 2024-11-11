/*============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-04-19
 *============================*/

// Reference:
// https://github.com/chriskohlhoff/asio/tree/master/asio/src/examples/cpp11/http/server

#include "server.hpp"
#include "types.hpp"
#include "utils/logger/logger.hpp"
#include <csignal>
#include <exception>
#include <utility>

namespace http::server {

error_code server::init(const char* host, const char* port) noexcept {
  try {
    signals.add(SIGINT);
    signals.add(SIGTERM);
#if defined(SIGQUIT)
    signals.add(SIGQUIT);
#endif // defined(SIGQUIT)

    do_await_stop();

    asio::ip::tcp::resolver resolver{this->io_cxt};
    asio::ip::tcp::endpoint endpoint = *resolver.resolve(host, port).begin();
    acceptor.open(endpoint.protocol());
    acceptor.set_option(asio::ip::tcp::acceptor::reuse_address(true));
    acceptor.bind(endpoint);
    acceptor.listen();

    do_accept();

    return error::OK;
  } catch (std::exception& err) {
    logger::error("server run error: %s", err.what());
    return error::SERVER_INIT_ERROR;
  }
}

void server::add_route(router&& route) noexcept {
  this->req_handler.add_route(std::forward<router&&>(route));
}

error_code server::run() noexcept {
  RP_TRY(this->req_handler.finalize());

  try {
    this->io_cxt.run();
    return error::OK;
  } catch (std::exception& err) {
    logger::error("server run error: %s", err.what());
    return error::SERVER_RUN_ERROR;
  }
}

void server::do_accept() noexcept {
  acceptor.async_accept(
      [this](std::error_code ec, asio::ip::tcp::socket socket) {
        if (!this->acceptor.is_open()) {
          return;
        }

        if (!ec) {
          this->conn_manager.start(std::make_shared<connection>(
              std::move(socket), this->conn_manager, this->req_handler
          ));
        }

        this->do_accept();
      }
  );
}

void server::do_await_stop() noexcept {
  signals.async_wait([this](std::error_code /*ec*/, int /*signo*/) {
    this->acceptor.close();
    this->conn_manager.stop_all();
  });
}

} // namespace http::server
