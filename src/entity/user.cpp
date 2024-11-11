/*============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-04-13
 *============================*/

#include "./user.hpp"
#include "types.hpp"
#include <cstring>

namespace entity {

// === User === //

error_code User::copy(const User& other) noexcept {
  this->id = other.id;
  RP_TRY(this->username.copy(other.username));
  RP_TRY(this->password.copy(other.password));
  return error::OK;
}

// === Getters === //

const uuid& User::get_id() const noexcept {
  return this->id;
}

const string& User::get_username() const noexcept {
  return this->username;
}

const string& User::get_password() const noexcept {
  return this->password;
}

// === Setters === //

void User::set_id(const uuid& id) noexcept {
  this->id = id;
}

void User::set_id(const string& id) noexcept {
  this->id = id;
}

void User::set_id(const c8* id) noexcept {
  this->id = id;
}

error_code User::set_username(const string& username) noexcept {
  if (username.is_empty()) {
    return error::USER_USERNAME_REQUIRED;
  }

  if (username.get_size() > User::USERNAME_MAX) {
    return error::USER_USERNAME_MAX_LENGTH;
  }

  return this->username.copy(username);
}

error_code User::set_username(const c8* username) noexcept {
  if (username == nullptr || username[0] == '\0') {
    return error::USER_USERNAME_REQUIRED;
  }

  if (strnlen(username, User::USERNAME_MAX + 1) > User::USERNAME_MAX) {
    return error::USER_USERNAME_MAX_LENGTH;
  }

  return this->username.copy(username);
}

error_code User::set_password(const string& password) noexcept {
  if (password.is_empty()) {
    return error::USER_PASSWORD_REQUIRED;
  }

  if (password.get_size() < User::PASSWORD_MIN) {
    return error::USER_PASSWORD_MIN_LENGTH;
  }

  if (username.get_size() > User::PASSWORD_MAX) {
    return error::USER_PASSWORD_MAX_LENGTH;
  }

  return this->password.copy(password);
}

error_code User::set_password(const c8* password) noexcept {
  if (password == nullptr || password[0] == '\0') {
    return error::USER_PASSWORD_REQUIRED;
  }

  const u32 size = strnlen(password, User::PASSWORD_MAX + 1);
  if (size < User::PASSWORD_MIN) {
    return error::USER_PASSWORD_MIN_LENGTH;
  }

  if (size > User::PASSWORD_MAX) {
    return error::USER_PASSWORD_MAX_LENGTH;
  }

  return this->password.copy(password);
}

error_code User::set_hashed_password(const string& hash) noexcept {
  return this->password.copy(hash);
}

error_code User::set_hashed_password(const c8* hash) noexcept {
  return this->password.copy(hash);
}

} // namespace entity
