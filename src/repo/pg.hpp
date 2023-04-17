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
      const types::string& db, const types::string& host, types::u16 port
  ) noexcept;
  types::opt_err init(
      const char* user, const char* pass, const char* db, const char* host,
      types::u16 port
  ) noexcept;

  PgManager(PgManager&& rhs) noexcept = default;
  PgManager& operator=(PgManager&& rhs) noexcept = default;

  ~PgManager() noexcept = default;

  // === Functions === //

  types::exp_err<types::shared_ptr<PgClient>> get_client() noexcept;
};

class PgClient {
private:
  PGconn* conn = nullptr;

public:
  friend PgManager;
};

} // namespace repo

