/*============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-04-15
 *============================*/

#include "config/types.hpp"
#include "entity/user.hpp"

namespace repo {

template <typename T> class User {
public:
  friend T;

  // === Create === //
  types::string add_user(const entity::User& user) noexcept {
    return static_cast<T*>(this)->add_user_impl(user);
  }
  types::string add_user_impl(const entity::User& user) noexcept = delete;

  // === Read === //
  entity::User get_user_by_id(const types::string& id) noexcept {
    return static_cast<T*>(this)->get_user_by_id_impl(id);
  }
  entity::User get_user_by_id(const char* id) noexcept {
    return static_cast<T*>(this)->get_user_by_id_impl(id);
  }
  entity::User get_user_by_id_impl(const types::string& id) noexcept = delete;
  entity::User get_user_by_id_impl(const char* id) noexcept = delete;

  // === Update === //
  
  // === Delete === //
};

} // namespace repo

