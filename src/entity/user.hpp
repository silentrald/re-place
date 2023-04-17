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

const types::i32 USERNAME_MAX = 60;
const types::i32 PASSWORD_MAX = 60;

class UserBuilder;

class User {
private:
  types::string id{};
  types::string username{};
  types::string password{};

public:
  friend UserBuilder;

  // === Constructor === //

  User() noexcept = default;
  User(const User&) = delete;
  User& operator=(const User&) = delete;

  [[nodiscard]] types::opt_err copy(const User& other) noexcept;

  User(User&& rhs) noexcept = default;
  User& operator=(User&& rhs) noexcept = default;

  ~User() noexcept = default;

  // === Getters === //

  [[nodiscard]] const types::string& get_id() const noexcept;
  [[nodiscard]] const types::string& get_username() const noexcept;
  [[nodiscard]] const types::string& get_password() const noexcept;
};

class UserBuilder {
private:
  types::string id{};
  types::string username{};
  types::string password{};

public:
  // === Constructor === //

  UserBuilder() noexcept = default;
  UserBuilder(const UserBuilder&) = delete;
  UserBuilder& operator=(const UserBuilder&) = delete;

  UserBuilder(UserBuilder&& rhs) noexcept = default;
  UserBuilder& operator=(UserBuilder&& rhs) noexcept = default;

  ~UserBuilder() noexcept = default;

  // === Id === //

  [[nodiscard]] types::opt_err set_id(const types::string& id) noexcept;
  void set_id(types::string&& id) noexcept;
  [[nodiscard]] types::opt_err set_id(const char* id) noexcept;

  // === Username === //

  [[nodiscard]] types::opt_err set_username(const types::string& username
  ) noexcept;
  void set_username(types::string&& username) noexcept;
  [[nodiscard]] types::opt_err set_username(const char* username) noexcept;

  // === Password === //

  [[nodiscard]] types::opt_err set_password(const types::string& password
  ) noexcept;
  void set_password(types::string&& password) noexcept;
  [[nodiscard]] types::opt_err set_password(const char* password) noexcept;

  [[nodiscard]] types::exp_err<User> build() noexcept;
};

} // namespace entity

#endif

