/*============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2024-12-01
 *============================*/

#include "./test.hpp"
#include "types.hpp"

#define TEST(function) success = function(data) && success;

i32 main(i32 argc, c8* argv[]) noexcept {
  test::Data data{};
  if (argc == 2) {
    // Just get the first argument for tags
    error_code code = data.set_tags(argv[1]);
    if (rp::is_error(code)) {
      printf("Could not initialize tags: %s", rp::get_error_string(code));
      return 1;
    }
  }

  bool success = true;

  // === Test Start === //

  TEST(test::test_crypto);

  // === Test End === //

  return success ? 0 : 1;
}
