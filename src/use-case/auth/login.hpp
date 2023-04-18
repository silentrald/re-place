/*============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-04-15
 *============================*/

#ifndef USE_CASE_AUTH_LOGIN_HPP
#define USE_CASE_AUTH_LOGIN_HPP

#include "config/types.hpp"
#include "repo/user.hpp"
#include <iostream>

namespace use_case::auth {

template <typename UserRepo> class Login {
private:
  repo::User<UserRepo>* user_repo = nullptr;

public:
  Login(UserRepo* user_repo) noexcept : user_repo(user_repo) {} // NOLINT

  types::opt_err execute(
      const types::string& username, const types::string& password
  ) noexcept {

    auto user = try_exp_err(this->user_repo->get_user_by_username(username));
    if (user.get_password() != password) {
      return types::error{"Password Mismatch", def_err_vals};
    }

    std::cout << "Logged In\n";

    return types::null;
  }
};

} // namespace use_case::auth

#endif

