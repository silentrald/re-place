/*============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2024-12-01
 *============================*/

#include "utils/crypto/crypto.hpp"
#include "./test.hpp"
#include <cstring>

bool test::test_crypto(Data& data) noexcept {
  TEST_INIT("Crypto");

  TEST_CASE("Password hashing", "[crypto]") {
    const c8* password = "password1";

    string hash = TEST_TRY_RETURN(crypto::hash_password(password));

    EXPECT(hash, password, test::ExpectComparator::NOT_EQUAL);
    EXPECT_TRUE(crypto::verify_password(hash, password));

    TEST_OK;
  }
  TEST_CASE_END();

  TEST_CASE("Password hashing with length", "[crypto]") {
    const c8* password = "password2";
    const u32 length = std::strlen(password);

    string hash = TEST_TRY_RETURN(crypto::hash_password(password, length));
    EXPECT_TRUE(crypto::verify_password(hash, password));

    TEST_OK;
  }
  TEST_CASE_END();

  TEST_END();
}
