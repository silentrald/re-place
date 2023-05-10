/*============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-05-10
 *============================*/

#ifndef CACHE_SESSION_HPP
#define CACHE_SESSION_HPP

#include "config/types.hpp"
#include "entity/user.hpp"

namespace cache {

template <typename Derived> class Session {
public:
  friend Derived;

  [[nodiscard]] exp_err<string> set_session(const entity::User& user) noexcept {
    return static_cast<Derived*>(this)->set_session_impl(user);
  }
  [[nodiscard]] exp_err<string> set_session_impl(const entity::User& user
  ) noexcept = delete;

  [[nodiscard]] exp_err<entity::User> get_session(const string& session_id
  ) noexcept {
    return static_cast<Derived*>(this)->get_session_impl(session_id);
  }
  [[nodiscard]] exp_err<entity::User> get_session_impl(const string& session_id
  ) noexcept = delete;
};

} // namespace cache

#endif

