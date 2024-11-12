/*============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-05-03
 *============================*/

#ifndef UTILS_LOGGER_HPP
#define UTILS_LOGGER_HPP

// #include "api/asio/request.hpp"
// #include "api/asio/response.hpp"

namespace logger {

enum class level {
  SILENT = 0,
  FATAL = 1,
  ERROR = 2,
  WARN = 3,
  INFO = 4,
  DEBUG = 5
};

void set_level(level lvl) noexcept;

void fatal(const char* message, ...) noexcept;
void error(const char* message, ...) noexcept;
void warn(const char* message, ...) noexcept;
void info(const char* message, ...) noexcept;
void debug(const char* message, ...) noexcept;

// void http(
//     const http::server::request& req, const http::server::response& res
// ) noexcept;

} // namespace logger

#endif
