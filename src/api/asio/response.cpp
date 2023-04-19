/*============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-04-19
 *============================*/

// See:
// https://github.com/chriskohlhoff/asio/tree/master/asio/src/examples/cpp11/http/server

#include "response.hpp"
#include "asio/buffer.hpp"
#include "config/types.hpp"
#include <cstring>

namespace http::server {

namespace status_strings {

const char* ok = "HTTP/1.0 200 OK\r\n";
const char* created = "HTTP/1.0 201 Created\r\n";
const char* accepted = "HTTP/1.0 202 Accepted\r\n";
const char* no_content = "HTTP/1.0 204 No Content\r\n";
const char* multiple_choices = "HTTP/1.0 300 Multiple Choices\r\n";
const char* moved_permanently = "HTTP/1.0 301 Moved Permanently\r\n";
const char* moved_temporarily = "HTTP/1.0 302 Moved Temporarily\r\n";
const char* not_modified = "HTTP/1.0 304 Not Modified\r\n";
const char* bad_request = "HTTP/1.0 400 Bad Request\r\n";
const char* unauthorized = "HTTP/1.0 401 Unauthorized\r\n";
const char* forbidden = "HTTP/1.0 403 Forbidden\r\n";
const char* not_found = "HTTP/1.0 404 Not Found\r\n";
const char* internal_server_error = "HTTP/1.0 500 Internal Server Error\r\n";
const char* not_implemented = "HTTP/1.0 501 Not Implemented\r\n";
const char* bad_gateway = "HTTP/1.0 502 Bad Gateway\r\n";
const char* service_unavailable = "HTTP/1.0 503 Service Unavailable\r\n";

const types::u32 ok_len = strlen(ok);
const types::u32 created_len = strlen(created);
const types::u32 accepted_len = strlen(accepted);
const types::u32 no_content_len = strlen(no_content);
const types::u32 multiple_choices_len = strlen(multiple_choices);
const types::u32 moved_permanently_len = strlen(moved_permanently);
const types::u32 moved_temporarily_len = strlen(moved_temporarily);
const types::u32 not_modified_len = strlen(not_modified);
const types::u32 bad_request_len = strlen(bad_request);
const types::u32 unauthorized_len = strlen(unauthorized);
const types::u32 forbidden_len = strlen(forbidden);
const types::u32 not_found_len = strlen(not_found);
const types::u32 internal_server_error_len = strlen(internal_server_error);
const types::u32 not_implemented_len = strlen(not_implemented);
const types::u32 bad_gateway_len = strlen(bad_gateway);
const types::u32 service_unavailable_len = strlen(service_unavailable);

asio::const_buffer to_buffer(response::status_type status) {
  switch (status) {
  case response::ok:
    return asio::buffer(ok, ok_len);
  case response::created:
    return asio::buffer(created, created_len);
  case response::accepted:
    return asio::buffer(accepted, accepted_len);
  case response::no_content:
    return asio::buffer(no_content, no_content_len);
  case response::multiple_choices:
    return asio::buffer(multiple_choices, multiple_choices_len);
  case response::moved_permanently:
    return asio::buffer(moved_permanently, moved_permanently_len);
  case response::moved_temporarily:
    return asio::buffer(moved_temporarily, moved_temporarily_len);
  case response::not_modified:
    return asio::buffer(not_modified, not_modified_len);
  case response::bad_request:
    return asio::buffer(bad_request, bad_request_len);
  case response::unauthorized:
    return asio::buffer(unauthorized, unauthorized_len);
  case response::forbidden:
    return asio::buffer(forbidden, forbidden_len);
  case response::not_found:
    return asio::buffer(not_found, not_found_len);
  case response::internal_server_error:
    return asio::buffer(internal_server_error, internal_server_error_len);
  case response::not_implemented:
    return asio::buffer(not_implemented, not_implemented_len);
  case response::bad_gateway:
    return asio::buffer(bad_gateway, bad_gateway_len);
  case response::service_unavailable:
    return asio::buffer(service_unavailable, service_unavailable_len);
  default:
    return asio::buffer(internal_server_error, internal_server_error_len);
  }
}

} // namespace status_strings

namespace misc_strings {

const char name_value_separator[] = {':', ' '}; // NOLINT
const char crlf[] = {'\r', '\n'};               // NOLINT

} // namespace misc_strings

std::vector<asio::const_buffer> response::to_buffers() {
  std::vector<asio::const_buffer> buffers;
  buffers.push_back(status_strings::to_buffer(status));
  for (std::size_t i = 0; i < headers.size(); ++i) {
    header& h = headers[i];
    buffers.push_back(asio::buffer(h.name.c_str(), h.name.size()));
    buffers.push_back(asio::buffer(misc_strings::name_value_separator));
    buffers.push_back(asio::buffer(h.value.c_str(), h.value.size()));
    buffers.push_back(asio::buffer(misc_strings::crlf));
  }
  buffers.push_back(asio::buffer(misc_strings::crlf));
  buffers.push_back(asio::buffer(content.c_str(), content.size()));
  return buffers;
}

namespace stock_replies {

const char* ok = "";
const char* created = "<html>"
                      "<head><title>Created</title></head>"
                      "<body><h1>201 Created</h1></body>"
                      "</html>";
const char* accepted = "<html>"
                       "<head><title>Accepted</title></head>"
                       "<body><h1>202 Accepted</h1></body>"
                       "</html>";
const char* no_content = "<html>"
                         "<head><title>No Content</title></head>"
                         "<body><h1>204 Content</h1></body>"
                         "</html>";
const char* multiple_choices = "<html>"
                               "<head><title>Multiple Choices</title></head>"
                               "<body><h1>300 Multiple Choices</h1></body>"
                               "</html>";
const char* moved_permanently = "<html>"
                                "<head><title>Moved Permanently</title></head>"
                                "<body><h1>301 Moved Permanently</h1></body>"
                                "</html>";
const char* moved_temporarily = "<html>"
                                "<head><title>Moved Temporarily</title></head>"
                                "<body><h1>302 Moved Temporarily</h1></body>"
                                "</html>";
const char* not_modified = "<html>"
                           "<head><title>Not Modified</title></head>"
                           "<body><h1>304 Not Modified</h1></body>"
                           "</html>";
const char* bad_request = "<html>"
                          "<head><title>Bad Request</title></head>"
                          "<body><h1>400 Bad Request</h1></body>"
                          "</html>";
const char* unauthorized = "<html>"
                           "<head><title>Unauthorized</title></head>"
                           "<body><h1>401 Unauthorized</h1></body>"
                           "</html>";
const char* forbidden = "<html>"
                        "<head><title>Forbidden</title></head>"
                        "<body><h1>403 Forbidden</h1></body>"
                        "</html>";
const char* not_found = "<html>"
                        "<head><title>Not Found</title></head>"
                        "<body><h1>404 Not Found</h1></body>"
                        "</html>";
const char* internal_server_error =
    "<html>"
    "<head><title>Internal Server Error</title></head>"
    "<body><h1>500 Internal Server Error</h1></body>"
    "</html>";
const char* not_implemented = "<html>"
                              "<head><title>Not Implemented</title></head>"
                              "<body><h1>501 Not Implemented</h1></body>"
                              "</html>";
const char* bad_gateway = "<html>"
                          "<head><title>Bad Gateway</title></head>"
                          "<body><h1>502 Bad Gateway</h1></body>"
                          "</html>";
const char* service_unavailable =
    "<html>"
    "<head><title>Service Unavailable</title></head>"
    "<body><h1>503 Service Unavailable</h1></body>"
    "</html>";

const char* to_string(response::status_type status) {
  switch (status) {
  case response::ok:
    return ok;
  case response::created:
    return created;
  case response::accepted:
    return accepted;
  case response::no_content:
    return no_content;
  case response::multiple_choices:
    return multiple_choices;
  case response::moved_permanently:
    return moved_permanently;
  case response::moved_temporarily:
    return moved_temporarily;
  case response::not_modified:
    return not_modified;
  case response::bad_request:
    return bad_request;
  case response::unauthorized:
    return unauthorized;
  case response::forbidden:
    return forbidden;
  case response::not_found:
    return not_found;
  case response::internal_server_error:
    return internal_server_error;
  case response::not_implemented:
    return not_implemented;
  case response::bad_gateway:
    return bad_gateway;
  case response::service_unavailable:
    return service_unavailable;
  default:
    return internal_server_error;
  }
}

} // namespace stock_replies

response response::stock_response(response::status_type status) {
  response rep;
  rep.status = status;
  types::err_code err = rep.content.copy(stock_replies::to_string(status));
  err = rep.headers.resize(2);
  err = rep.headers[0].name.copy("Content-Length");
  err = rep.headers[0].value.copy(std::to_string(rep.content.size()).c_str()
  ); // TODO: Converted
  err = rep.headers[1].name.copy("Content-Type");
  err = rep.headers[1].value.copy("text/html");
  return rep;
}

} // namespace http::server
