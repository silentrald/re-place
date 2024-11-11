/*============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-04-13
 *============================*/

#ifndef REPLACE_ENTITY_USER_HPP
#define REPLACE_ENTITY_USER_HPP

#include "types.hpp"

namespace entity {

// === Consts === //

class User {
public:
  // Configuration on CMake maybe
  inline static const i32 USERNAME_MAX = 60;
  inline static const i32 PASSWORD_MIN = 8;
  inline static const i32 PASSWORD_MAX = 60;

  // === Constructor === //

  User() noexcept = default;

  User(const User&) = delete;
  User& operator=(const User&) = delete;

  [[nodiscard]] error_code copy(const User& other) noexcept;

  User(User&& rhs) noexcept = default;
  User& operator=(User&& rhs) noexcept = default;

  ~User() noexcept = default;

  // === Getters === //

  [[nodiscard]] const uuid& get_id() const noexcept;
  [[nodiscard]] const string& get_username() const noexcept;
  [[nodiscard]] const string& get_password() const noexcept;

  // === Setters / Mutations === //

  void set_id(const uuid& id) noexcept;

  /**
   * This does not validate if the id is valid or not, either assume that it
   *   is already in uuid format coming from the database or generated properly
   *   in the uuid class
   **/
  void set_id(const string& id) noexcept;

  /**
   * This does not validate if the id is valid or not, either assume that it
   *   is already in uuid format coming from the database or generated properly
   *   in the uuid class
   **/
  void set_id(const c8* id) noexcept;

  error_code set_username(const string& username) noexcept;
  error_code set_username(const c8* username) noexcept;

  error_code set_password(const string& password) noexcept;
  error_code set_password(const c8* password) noexcept;

  error_code set_hashed_password(const string& hash) noexcept;
  error_code set_hashed_password(const c8* hash) noexcept;

private:
  uuid id;
  string username;
  // Hashed representation of the password.
  //   Do not store the real password of the user here.
  string password;
};

} // namespace entity

#endif
