/*============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-04-15
 *============================*/

#ifndef USE_CASE_AUTH_LOGIN_HPP
#define USE_CASE_AUTH_LOGIN_HPP

#include "cache/session.hpp"
#include "config/logger.hpp"
#include "config/types.hpp"
#include "ds/macro.hpp"
#include "repo/user.hpp"
#include <cstring>

namespace use_case::auth {

namespace login_err {

inline const char* const USERNAME_EMPTY = "Username is empty";
inline const char* const PASSWORD_EMPTY = "Password is empty";
inline const char* const PASSWORD_MISMATCH = "Password Mismatch";

} // namespace login_err

template <typename UserRepo, typename SessionCache> class Login {
public:
  opt_err init(
      repo::User<UserRepo>* user_repo, cache::Session<SessionCache>* session
  ) noexcept {
    if (user_repo == nullptr || session == nullptr) {
      // TODO:
      return error{"value error", def_err_vals};
    }

    this->user_repo = user_repo;
    this->session = session;

    return null;
  }

  [[nodiscard]] opt_err
  execute(const string& username, const string& password) noexcept {
    return this->execute(username.c_str(), password.c_str());
  }

  [[nodiscard]] opt_err
  execute(const char* username, const char* password) noexcept {
    if (username == nullptr || username[0] == '\0') {
      return error{login_err::USERNAME_EMPTY, def_err_vals};
    }

    if (password == nullptr || password[0] == '\0') {
      return error{login_err::PASSWORD_EMPTY, def_err_vals};
    }

    auto user = try_exp_err(this->user_repo->get_user_by_username(username));
    if (user.get_password() != password) {
      return error{login_err::PASSWORD_MISMATCH, def_err_vals};
    }

    // auto session_id = try_exp_err(this->session->set_session(user));
    // logger::info(session_id);
    logger::info("Logged in");

    return null;
  }

private:
  repo::User<UserRepo>* user_repo = nullptr;
  cache::Session<SessionCache>* session = nullptr;
};

} // namespace use_case::auth

#endif

