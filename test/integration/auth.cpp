/*============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2024-11-22
 *============================*/

#include "service/auth.hpp"
#include "./test.hpp"
#include "repo/session/def.hpp"
#include "repo/user/def.hpp"
#include "types.hpp"
#include "utils/crypto/crypto.hpp"

class MockUserRepo : public repo::User<MockUserRepo> {
public:
  [[nodiscard]] uuid get_user_id() const noexcept {
    return this->user.get_id();
  }

  [[nodiscard]] expected<uuid, error_code>
  add_user_impl(const entity::User& user) noexcept {
    RP_TRY(this->user.copy(user), rp::to_unexpected);

    uuid id{};
    id.generate_uuidv4();
    this->user.set_id(id);

    return this->user.get_id();
  }

  [[nodiscard]] expected<entity::User, error_code>
  get_user_by_username_impl(const string& username) noexcept {
    if (this->user.get_username() != username) {
      return unexpected<error_code>(error::NOT_FOUND);
    }

    entity::User user_copy{};
    RP_TRY(user_copy.copy(this->user), rp::to_unexpected);
    return std::move(user_copy);
  }

  [[nodiscard]] expected<entity::User, error_code>
  get_user_by_username_impl(const c8* username) noexcept {
    if (this->user.get_username() != username) {
      return unexpected<error_code>(error::NOT_FOUND);
    }

    entity::User user_copy{};
    RP_TRY(user_copy.copy(this->user), rp::to_unexpected);
    return std::move(user_copy);
  }

private:
  entity::User user;
};

class MockSessionRepo : public repo::Session<MockSessionRepo> {
public:
  [[nodiscard]] expected<uuid, error_code>
  set_session_impl(const entity::User& user) noexcept {
    RP_TRY(this->user.copy(user), rp::to_unexpected);

    this->session_id.generate_uuidv4();
    return this->session_id;
  }

  [[nodiscard]] expected<entity::User, error_code>
  get_user_impl(const uuid& session_id) noexcept {
    if (this->session_id.is_empty() || this->session_id != session_id) {
      return unexpected<error_code>(error::NOT_FOUND);
    }

    entity::User user_copy{};
    RP_TRY(user_copy.copy(this->user), rp::to_unexpected);
    return std::move(user_copy);
  }

  [[nodiscard]] error_code remove_session_impl(const uuid& session_id
  ) noexcept {
    return !this->session_id.is_empty() && this->session_id == session_id
               ? error::OK
               : error::NOT_FOUND;
  }

private:
  uuid session_id;
  entity::User user;
};

MockUserRepo mock_user_repo{};
MockSessionRepo mock_session_repo{};
service::Auth<MockUserRepo, MockSessionRepo> mock_auth_service{
    &mock_user_repo, &mock_session_repo
};

TEST_CASE_FUNCTION(test_mocked_register_and_login) {
  const c8* username = "username";
  const c8* password = "password";

  TEST_TRY(mock_auth_service.register_user(username, password));
  uuid session_id =
      TEST_TRY_RETURN(mock_auth_service.login(username, password));

  entity::User user = TEST_TRY_RETURN(mock_auth_service.get_user(session_id));

  EXPECT(user.get_username(), username, test::ExpectComparator::EQUAL);
  EXPECT_TRUE(crypto::verify_password(user.get_password(), password));

  TEST_OK;
}

bool test::test_auth(Data& data) noexcept {
  TEST_INIT("Auth");
  TEST_CASE(
      "Mock Register and Login", "[mock,auth]", test_mocked_register_and_login
  );
  TEST_END(__FILE_NAME__);
}
