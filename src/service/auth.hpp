/*============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-11-16
 *============================*/

#ifndef SERVICE_AUTH_HPP
#define SERVICE_AUTH_HPP

#include "repo/user/def.hpp"
#include "utils/crypto/crypto.hpp"
#include "utils/logger/logger.hpp"

namespace service {

template <typename UserRepo> class Auth {
public:
  explicit Auth(repo::User<UserRepo>* user_repo) noexcept {
    assert(user_repo != nullptr);
    this->user_repo = user_repo;
  }

  // === Login === //

  [[nodiscard]] error_code
  login(const string& username, const string& password) const noexcept {
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
  login(const char* username, const char* password) const noexcept {
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

  // === Register === //

  [[nodiscard]] error_code
  execute(const c8* username, const c8* password) const noexcept {
    entity::User user{};
    RP_TRY(user.set_username(username));
    RP_TRY(user.set_password(password));

    uuid id = RP_TRY_RETURN(this->user_repo->add_user(user), rp::to_error_code);

    logger::info("User [%s]:%s", id.get_string(), user.get_username().c_str());

    return error::OK;
  }

private:
  repo::User<UserRepo>* user_repo = nullptr;
};

} // namespace service

#endif
