/*============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-05-03
 *============================*/

#ifndef CONFIG_LOGGER_HPP
#define CONFIG_LOGGER_HPP

#include "api/asio/request.hpp"
#include "api/asio/response.hpp"
#include "config/types.hpp"

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

void fatal(const ds::error& err) noexcept;
void fatal(const string& msg) noexcept;
void fatal(const char* msg) noexcept;

void error(const ds::error& err) noexcept;
void error(const string& msg) noexcept;
void error(const char* msg) noexcept;

void warn(const ds::error& err) noexcept;
void warn(const string& msg) noexcept;
void warn(const char* msg) noexcept;

void info(
    const http::server::request& req, const http::server::response& res
) noexcept;
void info(const string& msg) noexcept;
void info(const char* msg) noexcept;

void debug(const string& msg) noexcept;
void debug(const char* msg) noexcept;

} // namespace logger

#endif

