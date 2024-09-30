/*============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-04-15
 *============================*/

#include "./pg.hpp"
#include "config/types.hpp"
#include "ds/macro.hpp"
#include "libpq-fe.h"
#include <cstdlib>

namespace repo {

// === PgManager === //

opt_err PgManager::init(
    const char* user, const char* pass, const char* db, const char* host,
    const char* port
) noexcept {
  try_opt(this->user.copy(user));
  try_opt(this->pass.copy(pass));
  try_opt(this->db.copy(db));
  try_opt(this->host.copy(host));
  try_opt(this->port.copy(port));
  return null;
}

exp_err<shared_ptr<PgClient>> PgManager::get_client() noexcept {
  shared_ptr<PgClient> client{};
  // TODO: Add pooling here
  try_opt_unexp(client.init());
  client->conn = PQsetdbLogin(
      this->host.c_str(), this->port.c_str(), nullptr, nullptr,
      this->db.c_str(), this->user.c_str(), this->pass.c_str()
  );
  if (PQstatus(client->conn) != CONNECTION_OK) {
    return unexp_err{error{
        PQerrorMessage(client->conn), err::DB_CONN_ERR,
        "Could not connect to the database", def_err_vals}};
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

opt_err
PgClient::prepare(const char* id, const char* query, i32 params) noexcept {
  auto* result = PQprepare(this->conn, id, query, params, nullptr);
  if (PQresultStatus(result) != PGRES_COMMAND_OK) {
    error err{
        PQresultErrorMessage(result), err::DB_PREPARE_ERR,
        "Could not prepare query", def_err_vals};
    PQclear(result);
    return err;
  }

  PQclear(result);
  return null;
}

exp_err<PgResult>
PgClient::execute(const char* id, vector<string>& values) noexcept {
  vector<const char*> cvalues{};

  try_opt_unexp(cvalues.reserve(values.size()));

  for (const auto& v : values) {
    static_cast<void>(cvalues.push_back(v.c_str()));
  }

  PgResult res{};
  res.result = PQexecPrepared(
      this->conn, id, values.size(), cvalues.data(), nullptr, nullptr, 0
  );
  if (PQresultStatus(res.result) != PGRES_TUPLES_OK) {
    return unexp_err{error{
        PQresultErrorMessage(res.result), err::DB_EXEC_ERR,
        "Could not execute query", def_err_vals}};
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

i32 PgResult::count() const noexcept {
  return this->size;
}

char* PgResult::get(i32 index) noexcept {
  return PQgetvalue(this->result, this->cursor, index);
}

bool PgResult::next() noexcept {
  return ++this->cursor < this->size;
}

} // namespace repo

