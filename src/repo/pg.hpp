/*============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-04-15
 *============================*/

#include "config/types.hpp"
#include <libpq-fe.h>

namespace repo {

class PgClient;
class PgResult;

class PgManager {
private:
  string user{};
  string pass{};

  string db{};
  string host{};
  string port{};

  i32 pool_size = 1;

  vector<shared_ptr<PgClient>> clients{};
  vector<shared_ptr<PgClient>> used{};

public:
  PgManager() noexcept = default;
  PgManager(const PgManager&) noexcept = delete;
  PgManager& operator=(const PgManager&) noexcept = delete;

  // === Constructor === //

  opt_err init(
      const string& user, const string& pass, const string& db,
      const string& host, string& port
  ) noexcept;
  opt_err init(
      const char* user, const char* pass, const char* db, const char* host,
      const char* port
  ) noexcept;

  PgManager(PgManager&& rhs) noexcept = default;
  PgManager& operator=(PgManager&& rhs) noexcept = default;

  ~PgManager() noexcept = default;

  // === Functions === //

  [[nodiscard]] exp_err<shared_ptr<PgClient>> get_client() noexcept;
};

class PgClient {
private:
  PGconn* conn = nullptr;

public:
  friend PgManager;

  PgClient() noexcept = default;
  PgClient(const PgClient&) noexcept = delete;
  PgClient& operator=(const PgClient&) noexcept = delete;

  PgClient(PgClient&& rhs) noexcept;
  PgClient& operator=(PgClient&& rhs) noexcept;
  ~PgClient() noexcept;

  [[nodiscard]] opt_err
  prepare(const char* id, const char* query, i32 params) noexcept;

  [[nodiscard]] exp_err<PgResult>
  execute(const char* id, vector<string>& values) noexcept;
};

class PgResult {
private:
  PGresult* result = nullptr;
  i32 cursor = 0;
  i32 size = 0;

public:
  friend PgClient;

  PgResult() noexcept = default;
  PgResult(const PgResult&) noexcept = delete;
  PgResult& operator=(const PgResult&) noexcept = delete;

  PgResult(PgResult&& rhs) noexcept;
  PgResult& operator=(PgResult&& rhs) noexcept;
  ~PgResult() noexcept;

  [[nodiscard]] i32 count() const noexcept;

  // TODO: Template return
  char* get(i32 index) noexcept;
  bool next() noexcept;
};

} // namespace repo

