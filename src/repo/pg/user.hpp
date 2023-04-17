/*============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-04-15
 *============================*/

#include "config/types.hpp"
#include "entity/user.hpp"
#include "repo/user.hpp"

namespace repo {

class UserPg : public User<UserPg> {
private:
public:
  UserPg() noexcept = default;
  UserPg(const UserPg&) noexcept = delete;
  UserPg& operator=(const UserPg&) noexcept = delete;

  // === Constructor === //
  types::opt_err init() noexcept;

  UserPg(UserPg&& rhs) noexcept = default;
  UserPg& operator=(UserPg&& rhs) noexcept = default;

  ~UserPg() noexcept = default;

  // === Create === //
  types::string add_user_impl(const entity::User& user) noexcept;

  // === Read === //
  entity::User get_user_by_id_impl(const types::string& id) noexcept;
  entity::User get_user_by_id_impl(const char* id) noexcept;
};

} // namespace repo

