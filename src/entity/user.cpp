/*============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-04-13
 *============================*/

#include "./user.hpp"
#include "config/types.hpp"
#include "ds/macro.hpp"
#include <utility>

namespace entity {

// TODO: Check for compile time creation for these
const char* USERNAME_EMPTY_ERR = "Username is required";
const char* USERNAME_MAX_ERR = "Username is too long (max 60 chars)";

const char* PASSWORD_MAX_ERR = "Password is too long (max 60 chars)";

// === User === //

types::opt_err User::copy(const User& other) noexcept {
  try_opt(this->id.copy(other.id));
  try_opt(this->username.copy(other.username));
  try_opt(this->password.copy(other.password));

  return types::null;
}

// === Getters === //

const types::string& User::get_id() const noexcept {
  return this->id;
}

const types::string& User::get_username() const noexcept {
  return this->username;
}

const types::string& User::get_password() const noexcept {
  return this->password;
}

// === UserBuilder === //

// === Id === //

types::opt_err UserBuilder::set_id(const types::string& id) noexcept {
  try_opt(this->id.copy(id));
  return types::null;
}

void UserBuilder::set_id(types::string&& id) noexcept {
  this->id = std::move(id);
}

types::opt_err UserBuilder::set_id(const char* id) noexcept {
  try_opt(this->id.copy(id));
  return types::null;
}

// === Username === //

types::opt_err UserBuilder::set_username(const types::string& username
) noexcept {
  try_opt(this->username.copy(username));
  return types::null;
}

void UserBuilder::set_username(types::string&& username) noexcept {
  this->username = std::move(username);
}

types::opt_err UserBuilder::set_username(const char* username) noexcept {
  try_opt(this->username.copy(username));
  return types::null;
}

// === Password === //

types::opt_err UserBuilder::set_password(const types::string& password
) noexcept {
  try_opt(this->password.copy(password));
  return types::null;
}

void UserBuilder::set_password(types::string&& password) noexcept {
  this->password = std::move(password);
}

types::opt_err UserBuilder::set_password(const char* password) noexcept {
  try_opt(this->password.copy(password));
  return types::null;
}

// END //

types::exp_err<User> UserBuilder::build() noexcept {

  // Validation //

  if (this->username.is_empty()) {
    return types::unexp_err(types::error{USERNAME_EMPTY_ERR, def_err_vals});
  }

  if (this->username.size() > USERNAME_MAX) {
    return types::unexp_err(types::error{USERNAME_MAX_ERR, def_err_vals});
  }

  if (this->password.size() > PASSWORD_MAX) {
    return types::unexp_err(types::error{PASSWORD_MAX_ERR, def_err_vals});
  }

  User user{};
  user.id = std::move(this->id);
  user.username = std::move(this->username);
  user.password = std::move(this->password);
  return user;
}

} // namespace entity
