/*============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-04-15
 *============================*/

// TODO: Check how to do async here

#ifndef REPLACE_REPO_PG_HPP
#define REPLACE_REPO_PG_HPP

#include "types.hpp"
#include <libpq-fe.h>

namespace repo {

class PgClient;
class PgResult;

class PgManager {
public:
  PgManager() noexcept = default;
  PgManager(const PgManager&) noexcept = delete;
  PgManager& operator=(const PgManager&) noexcept = delete;

  // === Constructor === //

  [[nodiscard]] error_code init(
      const string& user, const string& pass, const string& db,
      const string& host, string& port
  ) noexcept;
  [[nodiscard]] error_code init(
      const char* user, const char* pass, const char* db, const char* host,
      const char* port
  ) noexcept;

  PgManager(PgManager&& rhs) noexcept = default;
  PgManager& operator=(PgManager&& rhs) noexcept = default;

  ~PgManager() noexcept = default;

  // === Functions === //

  [[nodiscard]] expected<shared_ptr<PgClient>, error_code>
  get_client() noexcept;

private:
  string user;
  string pass;

  string db;
  string host;
  string port;

  i32 pool_size = 1;

  // TODO: Look into shared_ptr implementation if not broken
  vector<shared_ptr<PgClient>> clients{};
  vector<shared_ptr<PgClient>> used{};
};

class PgClient {
public:
  friend PgManager;

  PgClient() noexcept = default;
  PgClient(const PgClient&) noexcept = delete;
  PgClient& operator=(const PgClient&) noexcept = delete;

  PgClient(PgClient&& rhs) noexcept;
  PgClient& operator=(PgClient&& rhs) noexcept;
  ~PgClient() noexcept;

  [[nodiscard]] error_code
  prepare(const char* id, const char* query, i32 params) noexcept;

  [[nodiscard]] expected<PgResult, error_code>
  execute(const char* id, vector<string>& values) noexcept;

private:
  PGconn* conn = nullptr;
};

class PgResult {
public:
  friend PgClient;

  PgResult() noexcept = default;
  PgResult(const PgResult&) noexcept = delete;
  PgResult& operator=(const PgResult&) noexcept = delete;

  PgResult(PgResult&& rhs) noexcept;
  PgResult& operator=(PgResult&& rhs) noexcept;
  ~PgResult() noexcept;

  [[nodiscard]] i32 count() const noexcept;

  u32 get_u32(i32 index) noexcept;
  u64 get_u64(i32 index) noexcept;
  i32 get_i32(i32 index) noexcept;
  i64 get_i64(i32 index) noexcept;
  char* get_string(i32 index) noexcept;

  bool next() noexcept;

private:
  PGresult* result = nullptr;
  i32 cursor = 0;
  i32 size = 0;
};

} // namespace repo

#endif
