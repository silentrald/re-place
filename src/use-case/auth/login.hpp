/*============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-04-15
 *============================*/

#ifndef USE_CASE_AUTH_LOGIN_HPP
#define USE_CASE_AUTH_LOGIN_HPP

#include "entity/user.hpp"
#include "repo/user/def.hpp"
#include "types.hpp"
#include "utils/crypto/crypto.hpp"
#include "utils/logger/logger.hpp"
#include <cstring>

namespace use_case::auth {

// template <typename UserRepo, typename SessionCache> class Login {
template <typename UserRepo> class Login {
public:
  explicit Login(repo::User<UserRepo>* user_repo) noexcept {
    assert(user_repo != nullptr);
    this->user_repo = user_repo;
    // this->session = session;
  }

  [[nodiscard]] error_code
  execute(const string& username, const string& password) noexcept {
    if (username.is_empty()) {
      return error::USE_CASE_AUTH_LOGIN_USERNAME_EMPTY;
    }

    if (password.is_empty()) {
      return error::USE_CASE_AUTH_LOGIN_PASSWORD_EMPTY;
    }

    auto user = RP_TRY_RETURN(
        this->user_repo->get_user_by_username(), rp::to_error_code
    );
    if (user.get_password() != password) {
      return error::USE_CASE_AUTH_LOGIN_PASSWORD_MISMATCH;
    }

    // auto session_id = try_exp_err(this->session->set_session(user));
    // logger::info(session_id);
    logger::info("Logged in");

    return error::OK;
  }

  [[nodiscard]] error_code
  execute(const char* username, const char* password) noexcept {
    if (username == nullptr || username[0] == '\0') {
      return error::USE_CASE_AUTH_LOGIN_USERNAME_EMPTY;
    }

    if (password == nullptr || password[0] == '\0') {
      return error::USE_CASE_AUTH_LOGIN_PASSWORD_EMPTY;
    }

    entity::User user = RP_TRY_RETURN(
        this->user_repo->get_user_by_username(username), rp::to_error_code
    );

    if (!crypto::verify_password(user.get_password(), password)) {
      return error::USE_CASE_AUTH_LOGIN_PASSWORD_MISMATCH;
    }

    // auto session_id = try_exp_err(this->session->set_session(user));
    // logger::info(session_id);
    logger::info("Logged in");

    return error::OK;
  }

private:
  repo::User<UserRepo>* user_repo = nullptr;
  // cache::Session<SessionCache>* session = nullptr;
};

} // namespace use_case::auth
//
#endif
