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

namespace repo {

types::exp_err<types::shared_ptr<PgClient>> PgManager::get_client() noexcept {
  types::shared_ptr<PgClient> client{};
  if (!this->clients.is_empty()) {
    client = std::move(this->clients.pop_back().value());
  } else {
    // TODO: Add mutex here
    try_opt_unexp(client.init());
    client->conn = PQsetdbLogin(
        this->host.c_str(), this->port.c_str(), nullptr, nullptr,
        this->db.c_str(), this->user.c_str(), this->db.c_str()
    );
  }

  try_opt_unexp(this->used.push_back(client));
  return client;
}

} // namespace repo

