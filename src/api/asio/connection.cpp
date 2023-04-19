/*============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-04-19
 *============================*/

// See:
// https://github.com/chriskohlhoff/asio/tree/master/asio/src/examples/cpp11/http/server

#include "connection.hpp"
#include "api/asio/header.hpp"
#include "api/asio/request.hpp"
#include "api/asio/response.hpp"
#include "config/types.hpp"
#include "connection_manager.hpp"
#include "ds/types.hpp"
#include "llhttp.h"
#include "request_handler.hpp"
#include <algorithm>
#include <cctype>
#include <iostream>
#include <utility>
#include <vector>

namespace http::server {

connection::connection(
    asio::ip::tcp::socket socket, connection_manager& manager,
    request_handler& handler
)
    : socket(std::move(socket)), conn_manager(manager), req_handler(handler) {
  llhttp_settings_init(&this->settings);
  this->settings.on_url = [](llhttp_t* parser, const char* at,
                             unsigned long length) -> int {
    return ((request*)parser->data)->uri.copy(at, length) == types::SUCCESS
               ? HPE_OK
               : HPE_USER;
  };

  this->settings.on_header_field = [](llhttp_t* parser, const char* at,
                                      unsigned long length) -> int {
    header h{};
    types::err_code ec = h.name.copy(at, length);
    if (ec != types::SUCCESS) {
      return HPE_USER;
    }

    for (int i = 0; i < h.name.size(); ++i) {
      h.name[i] = std::tolower(h.name[i]);
    }

    return ((request*)parser->data)->headers.push_back(std::move(h)) ==
                   types::SUCCESS
               ? HPE_OK
               : HPE_USER;
  };

  this->settings.on_header_value = [](llhttp_t* parser, const char* at,
                                      unsigned long length) -> int {
    return ((request*)parser->data)
                       ->headers.back_unsafe()
                       .value.copy(at, length) == types::SUCCESS
               ? HPE_OK
               : HPE_USER;
  };

  this->settings.on_body = [](llhttp_t* parser, const char* at,
                              unsigned long length) -> int {
    return ((request*)parser->data)->body.copy(at, length) == types::SUCCESS
               ? HPE_OK
               : HPE_USER;
  };
  llhttp_init(&this->parser, HTTP_REQUEST, &this->settings);
  this->parser.data = &this->req;
}

void connection::start() {
  this->do_read();
}

void connection::stop() {
  this->socket.close();
}

void connection::do_read() noexcept {
  auto self(shared_from_this());
  this->socket.async_read_some(
      asio::buffer(this->buffer),
      [this, self](std::error_code ec, std::size_t bytes_transferred) {
        if (!ec) {
          auto err = llhttp_execute(
              &this->parser, this->buffer.data(), bytes_transferred
          );
          if (err != HPE_OK) {
            this->res = response::stock_response(response::bad_request);
            this->do_write();
            return;
          }

          if (this->parser.finish == 0) {
            this->req.method = this->parser.method;
            this->req.finish();

            this->req_handler.handle_request(this->req, this->res);
            this->do_write();
            return;
          }

          this->do_read();
        } else if (ec != asio::error::operation_aborted) {
          this->conn_manager.stop(shared_from_this());
        }
      }
  );
}

void connection::do_write() noexcept {
  auto self(shared_from_this());
  asio::async_write(
      this->socket, this->res.to_buffers(),
      [this, self](std::error_code ec, std::size_t) {
        if (!ec) {
          asio::error_code ignored_ec{};
          this->socket.shutdown(
              asio::ip::tcp::socket::shutdown_both, ignored_ec
          );
        }

        if (ec != asio::error::operation_aborted) {
          this->conn_manager.stop(shared_from_this());
        }
      }
  );
}

} // namespace http::server

