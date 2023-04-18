//
// connection.cpp
// ~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2023 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include "connection.hpp"
#include "api/asio/request.hpp"
#include "connection_manager.hpp"
#include "llhttp.h"
#include "request_handler.hpp"
#include <iostream>
#include <utility>
#include <vector>

namespace http::server {

connection::connection(
    asio::ip::tcp::socket socket, connection_manager& manager,
    request_handler& handler
)
    : socket_(std::move(socket)),
      connection_manager_(manager),
      request_handler_(handler) {
  llhttp_settings_init(&this->settings);
  this->settings.on_url = [](llhttp_t* parser, const char* at,
                             unsigned long length) -> int {
    ((request*)parser->data)->uri = std::string{at, at + length};
    return HPE_OK;
  };
  llhttp_init(&this->parser, HTTP_REQUEST, &this->settings);
  this->parser.data = &this->request_;
}

void connection::start() {
  do_read();
}

void connection::stop() {
  socket_.close();
}

void connection::do_read() {
  auto self(shared_from_this());
  socket_.async_read_some(
      asio::buffer(buffer_),
      [this, self](std::error_code ec, std::size_t bytes_transferred) {
        if (!ec) {
          std::cout << "Parsing: " << buffer_.data() << "\n";
          auto err =
              llhttp_execute(&this->parser, buffer_.data(), bytes_transferred);
          if (err != HPE_OK) {
            reply_ = reply::stock_reply(reply::bad_request);
            this->do_write();
            return;
          }

          if (this->parser.finish == 0) {
            this->request_.method =
                llhttp_method_name((llhttp_method_t)this->parser.method);

            request_handler_.handle_request(this->request_, this->reply_);
            this->do_write();
            return;
          }

          this->do_read();
        } else if (ec != asio::error::operation_aborted) {
          connection_manager_.stop(shared_from_this());
        }
      }
  );
}

void connection::do_write() {
  auto self(shared_from_this());
  asio::async_write(
      socket_, reply_.to_buffers(),
      [this, self](std::error_code ec, std::size_t) {
        if (!ec) {
          // Initiate graceful connection closure.
          asio::error_code ignored_ec;
          socket_.shutdown(asio::ip::tcp::socket::shutdown_both, ignored_ec);
        }

        if (ec != asio::error::operation_aborted) {
          connection_manager_.stop(shared_from_this());
        }
      }
  );
}

} // namespace http::server

