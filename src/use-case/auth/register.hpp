/*============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2024-11-11
 *============================*/

#ifndef USE_CASE_AUTH_REGISTER_HPP
#define USE_CASE_AUTH_REGISTER_HPP

#include "entity/user.hpp"
#include "repo/user/def.hpp"
#include "types.hpp"
#include "utils/logger/logger.hpp"

namespace use_case::auth {

template <typename UserRepo> class Register {
public:
  explicit Register(repo::User<UserRepo>* user_repo) noexcept {
    assert(user_repo != nullptr);
    this->user_repo = user_repo;
  }

  [[nodiscard]] error_code
  execute(const c8* username, const c8* password) noexcept {
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

} // namespace use_case::auth

#endif
