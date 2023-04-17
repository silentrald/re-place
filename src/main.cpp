#include "config/types.hpp"
#include "ds/macro.hpp"
#include "entity/user.hpp"
#include "llhttp.h"
#include <cstring>
#include <iostream>

types::opt_err wrapper() noexcept {
  entity::UserBuilder builder{};
  try_opt(builder.set_username(
      "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
      "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
  ));
  auto user = try_exp_err(builder.build());
  return types::null;
}

int main() {
  types::opt_err err = wrapper();
  if (err) {
    std::cout << err->get_msg() << '\n';
  }

  /* llhttp_t parser; */
  /* llhttp_settings_t settings; */

  /* /1* Initialize user callbacks and settings *1/ */
  /* llhttp_settings_init(&settings); */
  /* llhttp_init(&parser, HTTP_BOTH, &settings); */

  /* /1* Parse request! *1/ */
  /* constexpr const char* r[] = { */
  /*     "GET /favicon.ico HTTP/1.1\n", "Host: 0.0.0.0=5000\r\n", */
  /*     "User-Agent: Mozilla/5.0 (X11; U; Linux i686; en-US; rv:1.9) " */
  /*     "Gecko/2008061015 Firefox/3.0\r\n"}; */

  /* enum llhttp_errno err {}; */
  /* for (int i = 0; i < 3; ++i) { */
  /*   std::cout << "Parsing: " << r[i]; */
  /*   err = llhttp_execute(&parser, r[i], std::strlen(r[i])); */
  /*   if (err != HPE_OK) { */
  /*     std::cout << "Parse error: " << llhttp_errno_name(err) << ' ' */
  /*               << parser.reason << '\n'; */
  /*     return 0; */
  /*   } */
  /* } */
  /* llhttp_finish(&parser); */

  /* if (err == HPE_OK) { */
  /*   std::cout << "OK: <" << (int)parser.method << ' ' */
  /*             << llhttp_method_name((llhttp_method_t)parser.method) << ">\n";
   */
  /* } else { */
  /*   std::cout << "Parse error: " << llhttp_errno_name(err) << ' ' */
  /*             << parser.reason << '\n'; */
  /* } */
}

