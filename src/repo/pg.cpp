/*============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-04-15
 *============================*/

#include "./pg.hpp"
#include "libpq-fe.h"
#include "types.hpp"
#include <cstdio>
#include <cstdlib>

namespace repo {

// === PgManager === //

error_code PgManager::init(
    const char* user, const char* pass, const char* db, const char* host,
    const char* port
) noexcept {
  RP_TRY(this->user.copy(user));
  RP_TRY(this->pass.copy(pass));
  RP_TRY(this->db.copy(db));
  RP_TRY(this->host.copy(host));
  RP_TRY(this->port.copy(port));
  return error::OK;
}

expected<shared_ptr<PgClient>, error_code> PgManager::get_client() noexcept {
  shared_ptr<PgClient> client{};
  RP_TRY(client.init(), rp::to_unexpected);

  // TODO: Pooling
  client->conn = PQsetdbLogin(
      this->host.c_str(), this->port.c_str(), nullptr, nullptr,
      this->db.c_str(), this->user.c_str(), this->pass.c_str()
  );

  if (PQstatus(client->conn) != CONNECTION_OK) {
    return unexpected<error_code>{error::DB_CONNECTION_ERROR};
  }

  return std::move(client);
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

error_code
PgClient::prepare(const char* id, const char* query, i32 params) noexcept {
  PGresult* result = PQprepare(this->conn, id, query, params, nullptr);
  if (PQresultStatus(result) != PGRES_COMMAND_OK) {
    // TODO: Log the result
    // error err{
    //     PQresultErrorMessage(result), err::DB_PREPARE_ERR,
    //     "Could not prepare query", def_err_vals
    // };
    PQclear(result);
    return error::DB_PREPARE_ERROR;
  }

  PQclear(result);
  return error::OK;
}

expected<PgResult, error_code>
PgClient::execute(const c8* id, vector<string>& values) noexcept {
  vector<const c8*> string_values{};
  RP_TRY(string_values.reserve(values.get_size()), rp::to_unexpected);

  for (const auto& val : values) {
    static_cast<void>(string_values.push(val.c_str()));
  }

  PgResult res{};
  res.result = PQexecPrepared(
      this->conn, id, values.get_size(), string_values.get_data(), nullptr,
      nullptr, 0
  );
  if (PQresultStatus(res.result) != PGRES_TUPLES_OK) {
    // return unexp_err{error{
    //     PQresultErrorMessage(res.result), err::DB_EXEC_ERR,
    //     "Could not execute query", def_err_vals
    // }};
    return unexpected<error_code>{error::DB_EXECUTION_ERROR};
  }

  res.size = PQntuples(res.result);
  return std::move(res);
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

c8* PgResult::get_string(i32 index) noexcept {
  return PQgetvalue(this->result, this->cursor, index);
}

bool PgResult::next() noexcept {
  return ++this->cursor < this->size;
}

} // namespace repo
