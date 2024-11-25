/*============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-11-16
 *============================*/

#ifndef SERVICE_AUTH_HPP
#define SERVICE_AUTH_HPP

#include "entity/user.hpp"
#include "repo/session/def.hpp"
#include "repo/user/def.hpp"
#include "types.hpp"
#include "utils/crypto/crypto.hpp"
#include "utils/logger/logger.hpp"

namespace service {

template <typename UserRepo, typename SessionRepo> class Auth {
public:
  explicit Auth(
      repo::User<UserRepo>* user_repo, repo::Session<SessionRepo>* session_repo
  ) noexcept {
    assert(user_repo != nullptr);
    assert(session_repo != nullptr);

    this->user_repo = user_repo;
    this->session_repo = session_repo;
  }

  // === Login === //

  /**
   * @return session id
   **/
  [[nodiscard]] expected<uuid, error_code>
  login(const string& username, const string& password) const noexcept {
    if (username.is_empty()) {
      return unexpected<error_code>{error::USE_CASE_AUTH_LOGIN_USERNAME_EMPTY};
    }

    if (password.is_empty()) {
      return unexpected<error_code>{error::USE_CASE_AUTH_LOGIN_PASSWORD_EMPTY};
    }

    auto user = RP_TRY_RETURN(
        this->user_repo->get_user_by_username(), rp::to_unexpected
    );
    if (!crypto::verify_password(user.get_password(), password)) {
      return unexpected<error_code>{error::USE_CASE_AUTH_LOGIN_PASSWORD_MISMATCH
      };
    }

    uuid session_id = RP_TRY_RETURN(this->session_repo->set_session(user));
    logger::info("Logged in %s", session_id.get_string());

    return std::move(session_id);
  }

  /**
   * @return session id
   **/
  [[nodiscard]] expected<uuid, error_code>
  login(const char* username, const char* password) const noexcept {
    if (username == nullptr || username[0] == '\0') {
      return unexpected<error_code>{error::USE_CASE_AUTH_LOGIN_USERNAME_EMPTY};
    }

    if (password == nullptr || password[0] == '\0') {
      return unexpected<error_code>{error::USE_CASE_AUTH_LOGIN_PASSWORD_EMPTY};
    }

    entity::User user = RP_TRY_RETURN(
        this->user_repo->get_user_by_username(username), rp::to_unexpected
    );
    if (!crypto::verify_password(user.get_password(), password)) {
      return unexpected<error_code>{error::USE_CASE_AUTH_LOGIN_PASSWORD_MISMATCH
      };
    }

    uuid session_id = RP_TRY_RETURN(this->session_repo->set_session(user));
    logger::info("Logged in %s", session_id.get_string());

    return std::move(session_id);
  }

  // === Register === //

  [[nodiscard]] error_code
  register_user(const c8* username, const c8* password) const noexcept {
    entity::User user{};
    RP_TRY(user.set_username(username));
    RP_TRY(user.set_password(password));

    uuid id = RP_TRY_RETURN(this->user_repo->add_user(user), rp::to_error_code);

    logger::info("User [%s]:%s", id.get_string(), user.get_username().c_str());

    return error::OK;
  }

  // === Logout === //

  [[nodiscard]] error_code logout(const uuid& session_id) const noexcept {
    RP_TRY(this->session_repo->remove_session(session_id));
    return error::OK;
  }

  // TODO: Move this out of auth service
  [[nodiscard]] expected<entity::User, error_code>
  get_user(const uuid& session_id) const noexcept {
    return this->session_repo->get_user(session_id);
  }

private:
  repo::User<UserRepo>* user_repo = nullptr;
  repo::Session<SessionRepo>* session_repo = nullptr;
};

} // namespace service

#endif
