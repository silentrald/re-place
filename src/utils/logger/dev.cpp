/*============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2024-11-03
 *============================*/

#include "./logger.hpp"
#include "types.hpp"
#include <bits/types/struct_timeval.h>
#include <cstdarg>
#include <cstdio>
#include <ctime>
#include <mutex> // TODO: Remove this
#include <sys/time.h>

// Colors
const char* const TEXT_WHITE = "\e[97m";
const char* const TEXT_BLACK = "\e[30m";
const char* const TEXT_RED = "\x1B[31m";
const char* const TEXT_GREEN = "\x1B[32m";
const char* const TEXT_YELLOW = "\x1B[33m";
const char* const TEXT_BLUE = "\x1B[34m";
const char* const TEXT_VIOLET = "\x1B[93m";
const char* const TEXT_MAGENTA = "\e[95m";
const char* const BG_WHITE = "\e[107m";
const char* const BG_RED = "\e[41m";
const char* const TEXT_BOLD = "\033[1m";
const char* const TEXT_NORMAL = "\033[0m";
const char* const RESET = "\x1B[0m";

const char* const TIMESTAMP_FMT = "YYYY-MM-DDTHH:mm:ss.SSS";
const i32 TIMESTAMP_LENGTH = std::strlen(TIMESTAMP_FMT);
char timestamp[TIMESTAMP_LENGTH]; // NOLINT

const char* const FATAL_MSG = "fatal";
const char* const ERROR_MSG = "error";
const char* const WARN_MSG = "warn";
const char* const INFO_MSG = "info";
const char* const DEBUG_MSG = "debug";

logger::level log_level = logger::level::DEBUG;
std::mutex log_mutex{};

const char* get_timestamp() noexcept {
  timeval current = {};
  gettimeofday(&current, nullptr);

  static_cast<void>(strftime(
      timestamp, TIMESTAMP_LENGTH, "%Y-%m-%dT%H:%M:%S",
      localtime(&current.tv_sec) // NOLINT
  ));
  timestamp[sizeof "YYYY-MM-DDTHH:mm:ss"] = '\0';

  // NOLINTNEXTLINE
  sprintf(timestamp, "%s.%03ld", timestamp, current.tv_usec / 1000);
  return timestamp;
}

inline void print_header(
    const char* level, const char* level_fg, const char* level_bg
) noexcept {
  printf(
      "%s %s%s%s%s:%s ", get_timestamp(), TEXT_BOLD, level_fg, level_bg, level,
      RESET
  );
}

inline void print_header(const char* level, const char* level_fg) noexcept {
  printf("%s %s%s%s:%s ", get_timestamp(), TEXT_BOLD, level_fg, level, RESET);
}

void logger::set_level(logger::level lvl) noexcept {
  log_level = lvl;
}

// NOLINTNEXTLINE
void logger::fatal(const char* message, ...) noexcept {
  if (log_level < level::FATAL)
    return;

  log_mutex.lock();

  print_header(FATAL_MSG, TEXT_WHITE, BG_WHITE);

  va_list args{};
  va_start(args, message);
  vprintf(message, args);
  va_end(args);

  printf("\n");

  log_mutex.unlock();
}

// NOLINTNEXTLINE
void logger::error(const char* message, ...) noexcept {
  if (log_level < level::ERROR)
    return;

  log_mutex.lock();

  print_header(ERROR_MSG, TEXT_RED);

  va_list args{};
  va_start(args, message);
  vprintf(message, args);
  va_end(args);

  printf("\n");

  log_mutex.unlock();
}

// NOLINTNEXTLINE
void logger::warn(const char* message, ...) noexcept {
  if (log_level < level::WARN)
    return;

  log_mutex.lock();

  print_header(WARN_MSG, TEXT_YELLOW);

  va_list args{};
  va_start(args, message);
  vprintf(message, args);
  va_end(args);

  printf("\n");

  log_mutex.unlock();
}

// NOLINTNEXTLINE
void logger::info(const char* message, ...) noexcept {
  if (log_level < level::INFO)
    return;

  log_mutex.lock();

  print_header(INFO_MSG, TEXT_GREEN);

  va_list args{};
  va_start(args, message);
  vprintf(message, args);
  va_end(args);

  printf("\n");

  log_mutex.unlock();
}

// NOLINTNEXTLINE
void logger::debug(const char* message, ...) noexcept {
  if (log_level < level::DEBUG)
    return;

  log_mutex.lock();

  print_header(DEBUG_MSG, TEXT_VIOLET);

  va_list args{};
  va_start(args, message);
  vprintf(message, args);
  va_end(args);

  printf("\n");

  log_mutex.unlock();
}

// const char* get_status_color(i32 status) {
//   if (status >= 200 && status < 300) { // 200s Most Likely
//     return TEXT_GREEN;
//   }
//
//   if (status >= 400) { // 400s & 500s // Likely
//     return TEXT_RED;
//   }
//
//   // 100s & 300s Less Likely
//   return TEXT_BLUE;
// }

// TODO: Change to http()
// void info(
//     const http::server::request& req, const http::server::response& res
// ) noexcept {
//   if (log_level < level::INFO)
//     return;
//
//   log_mutex.lock();
//
//   print_info_header();
//   // TODO: Follow this format:
//   https://en.wikipedia.org/wiki/Common_Log_Format printf(
//       "%s %s %s%d%s %d\n", llhttp_method_name((llhttp_method)req.method),
//       req.uri.c_str(), get_status_color(res.status), res.status, RESET,
//       res.content.size()
//   );
//
//   log_mutex.unlock();
// }
