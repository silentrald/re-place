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
  types::string user{};
  types::string pass{};

  types::string db{};
  types::string host{};
  types::string port{};

  types::i32 pool_size = 1;

  types::vector<types::shared_ptr<PgClient>> clients{};
  types::vector<types::shared_ptr<PgClient>> used{};

public:
  PgManager() noexcept = default;
  PgManager(const PgManager&) noexcept = delete;
  PgManager& operator=(const PgManager&) noexcept = delete;

  // === Constructor === //

  types::opt_err init(
      const types::string& user, const types::string& pass,
      const types::string& db, const types::string& host, types::string& port
  ) noexcept;
  types::opt_err init(
      const char* user, const char* pass, const char* db, const char* host,
      const char* port
  ) noexcept;

  PgManager(PgManager&& rhs) noexcept = default;
  PgManager& operator=(PgManager&& rhs) noexcept = default;

  ~PgManager() noexcept = default;

  // === Functions === //

  [[nodiscard]] types::exp_err<types::shared_ptr<PgClient>>
  get_client() noexcept;
};

class PgClient {
private:
  PGconn* conn = nullptr;

public:
  friend PgManager;

  PgClient() noexcept = default;
  PgClient(const PgClient&) noexcept = delete;
  PgClient& operator=(const PgClient&) noexcept = delete;

  PgClient(PgClient&& rhs) noexcept : conn(rhs.conn) {
    rhs.conn = nullptr;
  }

  PgClient& operator=(PgClient&& rhs) noexcept {
    if (&rhs == this) {
      return *this;
    }

    this->conn = rhs.conn;
    rhs.conn = nullptr;

    return *this;
  }

  ~PgClient() noexcept {
    if (this->conn) {
      PQfinish(this->conn);
      this->conn = nullptr;
    }
  }

  [[nodiscard]] types::opt_err
  prepare(const char* id, const char* query, types::i32 params) noexcept;

  [[nodiscard]] types::exp_err<PgResult>
  execute(const char* id, types::vector<types::string>& values) noexcept;
};

class PgResult {
private:
  PGresult* result = nullptr;
  types::i32 cursor = 0;
  types::i32 size = 0;

public:
  friend PgClient;

  PgResult() noexcept = default;
  PgResult(const PgResult&) noexcept = delete;
  PgResult& operator=(const PgResult&) noexcept = delete;

  PgResult(PgResult&& rhs) noexcept
      : result(rhs.result), cursor(rhs.cursor), size(rhs.size) {
    rhs.result = nullptr;
  }

  PgResult& operator=(PgResult&& rhs) noexcept {
    if (&rhs == this) {
      return *this;
    }

    this->result = rhs.result;
    this->cursor = rhs.cursor;
    this->size = rhs.size;

    rhs.result = nullptr;

    return *this;
  }

  ~PgResult() noexcept {
    if (this->result) {
      PQclear(this->result);
      this->result = nullptr;
    }
  }

  [[nodiscard]] types::i32 count() const noexcept {
    return this->size;
  }

  // TODO: Template return
  char* get(types::i32 index) noexcept {
    return PQgetvalue(this->result, this->cursor, index);
  }

  bool next() noexcept {
    return ++this->cursor < this->size;
  }
};

} // namespace repo

