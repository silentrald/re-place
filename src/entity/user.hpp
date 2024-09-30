/*============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-04-13
 *============================*/

#ifndef ENTITY_USER_HPP
#define ENTITY_USER_HPP

#include "config/types.hpp"

namespace entity {

// === Consts === //

const i32 USERNAME_MAX = 60;
const i32 PASSWORD_MAX = 60;

class UserBuilder;

class User {
private:
  string id;
  string username;
  string password;

public:
  friend UserBuilder;

  // === Constructor === //

  User() noexcept = default;
  User(const User&) = delete;
  User& operator=(const User&) = delete;

  [[nodiscard]] opt_err copy(const User& other) noexcept;

  User(User&& rhs) noexcept = default;
  User& operator=(User&& rhs) noexcept = default;

  ~User() noexcept = default;

  // === Getters === //

  [[nodiscard]] const string& get_id() const noexcept;
  [[nodiscard]] const string& get_username() const noexcept;
  [[nodiscard]] const string& get_password() const noexcept;
};

class UserBuilder {
private:
  string id;
  string username;
  string password;

public:
  // === Constructor === //

  UserBuilder() noexcept = default;
  UserBuilder(const UserBuilder&) = delete;
  UserBuilder& operator=(const UserBuilder&) = delete;

  UserBuilder(UserBuilder&& rhs) noexcept = default;
  UserBuilder& operator=(UserBuilder&& rhs) noexcept = default;

  ~UserBuilder() noexcept = default;

  // === Id === //

  [[nodiscard]] opt_err set_id(const string& id) noexcept;
  void set_id(string&& id) noexcept;
  [[nodiscard]] opt_err set_id(const char* id) noexcept;

  // === Username === //

  [[nodiscard]] opt_err set_username(const string& username) noexcept;
  void set_username(string&& username) noexcept;
  [[nodiscard]] opt_err set_username(const char* username) noexcept;

  // === Password === //

  [[nodiscard]] opt_err set_password(const string& password) noexcept;
  void set_password(string&& password) noexcept;
  [[nodiscard]] opt_err set_password(const char* password) noexcept;

  [[nodiscard]] exp_err<User> build() noexcept;
};

} // namespace entity

#endif
