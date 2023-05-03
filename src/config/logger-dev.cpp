/*============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-05-03
 *============================*/

#include "./logger.hpp"
#include "config/types.hpp"
#include "ds-error/error.hpp"
#include <bits/types/struct_timeval.h>
#include <cstdio>
#include <ctime>
#include <sys/time.h>

namespace logger {

// TODO: Add the mutex here

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

level log_level = level::DEBUG;
mutex log_mutex{};

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

void set_level(level lvl) noexcept {
  log_level = lvl;
}

void print_error(const ds::error& msg) noexcept {
  printf(
      "%s\n# 0  > %s:%d\n", msg.get_msg(), msg.get_def_file(),
      msg.get_def_line()
  );

  ds::error_location* loc = nullptr;
  for (i32 i = 0; i < msg.get_size(); ++i) {
    loc = msg.get_location(i);
    printf("# %2d > %s:%d\n", i + 1, loc->file, loc->line);
  }
}

// === FATAL === //
inline void print_fatal_header() noexcept {
  print_header(FATAL_MSG, TEXT_WHITE, BG_WHITE);
}

void fatal(const ds::error& err) noexcept {
  if (log_level < level::FATAL)
    return;

  log_mutex.lock();

  print_fatal_header();
  print_error(err);

  log_mutex.unlock();
}

void fatal(const string& msg) noexcept {
  fatal(msg.c_str());
}

void fatal(const char* msg) noexcept {
  if (log_level < level::FATAL)
    return;

  log_mutex.lock();

  print_fatal_header();
  printf("%s\n", msg);

  log_mutex.unlock();
}

// === ERROR === //
inline void print_error_header() noexcept {
  print_header(ERROR_MSG, TEXT_RED);
}

void error(const ds::error& err) noexcept {
  if (log_level < level::ERROR)
    return;

  log_mutex.lock();

  print_error_header();
  print_error(err);

  log_mutex.unlock();
}

void error(const string& msg) noexcept {
  error(msg.c_str());
}

void error(const char* msg) noexcept {
  if (log_level < level::ERROR)
    return;

  log_mutex.lock();

  print_error_header();
  printf("%s\n", msg);

  log_mutex.unlock();
}

// === WARN === //
inline void print_warn_header() noexcept {
  print_header(WARN_MSG, TEXT_YELLOW);
}

void warn(const ds::error& err) noexcept {
  if (log_level < level::WARN)
    return;

  log_mutex.lock();

  print_warn_header();
  print_error(err);

  log_mutex.unlock();
}

void warn(const string& msg) noexcept {
  warn(msg.c_str());
}

void warn(const char* msg) noexcept {
  if (log_level < level::WARN)
    return;

  log_mutex.lock();

  print_warn_header();
  printf("%s\n", msg);

  log_mutex.unlock();
}

// === INFO === //
inline void print_info_header() noexcept {
  print_header(INFO_MSG, TEXT_GREEN);
}

void info(const string& msg) noexcept {
  info(msg.c_str());
}

void info(const char* msg) noexcept {
  if (log_level < level::INFO)
    return;

  log_mutex.lock();

  print_info_header();
  printf("%s\n", msg);

  log_mutex.unlock();
}

// === DEBUG === //
inline void print_debug_header() noexcept {
  print_header(DEBUG_MSG, TEXT_VIOLET);
}

void debug(const string& msg) noexcept {
  debug(msg.c_str());
}

void debug(const char* msg) noexcept {
  if (log_level < level::DEBUG)
    return;

  log_mutex.lock();

  print_debug_header();
  printf("%s\n", msg);

  log_mutex.unlock();
}

} // namespace logger

