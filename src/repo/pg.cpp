/*============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-04-15
 *============================*/

#include "./pg.hpp"
#include "config/types.hpp"
#include "ds/macro.hpp"
#include "ds/types.hpp"
#include "libpq-fe.h"
#include <cstdlib>

namespace repo {

// === PgManager === //

types::opt_err PgManager::init(
    const char* user, const char* pass, const char* db, const char* host,
    const char* port
) noexcept {
  auto err = this->user.copy(user);
  err = this->pass.copy(pass);
  err = this->db.copy(db);
  err = this->host.copy(host);
  err = this->port.copy(port);
  return types::null;
}

types::exp_err<types::shared_ptr<PgClient>> PgManager::get_client() noexcept {
  types::shared_ptr<PgClient> client{};
  // TODO: Add pooling here
  if (client.init() != types::SUCCESS) {
    return types::unexp_err{types::error{"Bad Allocation", def_err_vals}};
  }
  client->conn = PQsetdbLogin(
      this->host.c_str(), this->port.c_str(), nullptr, nullptr,
      this->db.c_str(), this->user.c_str(), this->pass.c_str()
  );
  if (PQstatus(client->conn) != CONNECTION_OK) {
    return types::unexp_err{types::error{
        PQerrorMessage(client->conn), "Could not connect to the database",
        def_err_vals}};
  }
  return client;
}

// === PgClient === //

PgClient::PgClient(PgClient&& rhs) noexcept : conn(rhs.conn) {
  rhs.conn = nullptr;
}

PgClient& PgClient::operator=(PgClient&& rhs) noexcept {
  if (&rhs == this) {
    return *this;
  }

  this->conn = rhs.conn;
  rhs.conn = nullptr;

  return *this;
}

PgClient::~PgClient() noexcept {
  if (this->conn) {
    PQfinish(this->conn);
    this->conn = nullptr;
  }
}

types::opt_err PgClient::prepare(
    const char* id, const char* query, types::i32 params
) noexcept {
  auto* result = PQprepare(this->conn, id, query, params, nullptr);
  if (PQresultStatus(result) != PGRES_COMMAND_OK) {
    types::error err{
        PQresultErrorMessage(result), "Could not prepare query", def_err_vals};
    PQclear(result);
    return err;
  }

  PQclear(result);
  return types::null;
}

types::exp_err<PgResult> PgClient::execute(
    const char* id, types::vector<types::string>& values
) noexcept {
  types::vector<const char*> cvalues{};

  auto ec = cvalues.reserve(values.size());
  if (ec != types::SUCCESS) {
    return types::unexp_err{types::error{"Bad Allocation", def_err_vals}};
  }

  for (const auto& v : values) {
    ec = cvalues.push_back(v.c_str());
  }

  PgResult res{};
  res.result = PQexecPrepared(
      this->conn, id, values.size(), cvalues.data(), nullptr, nullptr, 0
  );
  if (PQresultStatus(res.result) != PGRES_TUPLES_OK) {
    return types::unexp_err{types::error{
        PQresultErrorMessage(res.result), "Could not execute query",
        def_err_vals}};
  }

  res.size = PQntuples(res.result);
  return res;
}

// === PgResult === //

PgResult::PgResult(PgResult&& rhs) noexcept
    : result(rhs.result), cursor(rhs.cursor), size(rhs.size) {
  rhs.result = nullptr;
}

PgResult& PgResult::operator=(PgResult&& rhs) noexcept {
  if (&rhs == this) {
    return *this;
  }

  this->result = rhs.result;
  this->cursor = rhs.cursor;
  this->size = rhs.size;

  rhs.result = nullptr;

  return *this;
}

PgResult::~PgResult() noexcept {
  if (this->result) {
    PQclear(this->result);
    this->result = nullptr;
  }
}

types::i32 PgResult::count() const noexcept {
  return this->size;
}

char* PgResult::get(types::i32 index) noexcept {
  return PQgetvalue(this->result, this->cursor, index);
}

bool PgResult::next() noexcept {
  return ++this->cursor < this->size;
}

} // namespace repo

