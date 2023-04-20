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
#include <cstdio>
#include <utility>

namespace http::server {

connection::connection(
    asio::ip::tcp::socket socket, connection_manager& manager,
    request_handler& handler
)
    : socket(std::move(socket)), conn_manager(manager), req_handler(handler) {
  llhttp_settings_init(&this->settings);

  auto append_tmp = [](llhttp_t* parser, const char* at,
                       unsigned long length) -> int {
    return ((req_data*)parser->data)->tmp.append(at, length) == types::SUCCESS
               ? HPE_OK
               : HPE_USER;
  };
  this->settings.on_url = append_tmp;
  this->settings.on_header_field = append_tmp;
  this->settings.on_header_value = append_tmp;

  this->settings.on_url_complete = [](llhttp_t* parser) -> int {
    auto* data = (req_data*)parser->data;
    data->req->uri = std::move(data->tmp);
    return HPE_OK;
  };

  this->settings.on_header_field_complete = [](llhttp_t* parser) -> int {
    auto* data = (req_data*)parser->data;

    header h{};
    h.name = std::move(data->tmp);
    for (int i = 0; i < h.name.size(); ++i) {
      h.name[i] = std::tolower(h.name[i]);
    }
    return data->req->headers.push_back(std::move(h)) == types::SUCCESS
               ? HPE_OK
               : HPE_USER;
  };

  this->settings.on_header_value_complete = [](llhttp_t* parser) -> int {
    auto* data = (req_data*)parser->data;
    data->req->headers.back_unsafe().value = std::move(data->tmp);
    return HPE_OK;
  };

  this->settings.on_body = [](llhttp_t* parser, const char* at,
                              unsigned long length) -> int {
    return ((req_data*)parser->data)->req->body.append(at, length) ==
                   types::SUCCESS
               ? HPE_OK
               : HPE_USER;
  };
  llhttp_init(&this->parser, HTTP_REQUEST, &this->settings);

  this->data.req = &this->req;
  this->parser.data = &this->data;
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
            llhttp_finish(&this->parser);

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

