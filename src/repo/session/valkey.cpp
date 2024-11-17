/*============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2024-11-17
 *============================*/

#include "./valkey.hpp"
#include "entity/user.hpp"
#include "types.hpp"
#include <cassert>
#include <cstring>

#define PREFIX "rp-ses:"

namespace repo {

inline const u32 PREFIX_OFFSET = std::strlen(PREFIX);

SessionValkey::SessionValkey(ValkeyManager* manager) noexcept {
  assert(manager != nullptr);
  this->manager = manager;
}

expected<uuid, error_code>
SessionValkey::set_session_impl(const entity::User& user) noexcept {
  ValkeyClient client =
      RP_TRY_RETURN(this->manager->get_client(), rp::to_unexpected);

  // TODO: Integrate with Rapidjson
  string json{};
  RP_TRY(json.append(R"({"id":")"), rp::to_unexpected);
  RP_TRY(json.append(user.get_id().get_string()), rp::to_unexpected);
  RP_TRY(json.append(R"(","username":")"), rp::to_unexpected);
  RP_TRY(json.append(user.get_username().c_str()), rp::to_unexpected);
  RP_TRY(json.append(R"("})"), rp::to_unexpected);

  uuid session_id{};
  session_id.generate_uuidv4();

  c8 key[PREFIX_OFFSET + uuid::SIZE + 1] = PREFIX; // NOLINT
  std::strncpy(key + PREFIX_OFFSET, session_id.get_string(), uuid::SIZE);

  RP_TRY(client.set_string(key, json.c_str(), 3600), rp::to_unexpected);
  return session_id;
}

expected<entity::User, error_code>
SessionValkey::get_user_impl(const uuid& session_id) noexcept {
  ValkeyClient client =
      RP_TRY_RETURN(this->manager->get_client(), rp::to_unexpected);

  c8 key[PREFIX_OFFSET + uuid::SIZE + 1] = PREFIX; // NOLINT
  std::strncpy(key + PREFIX_OFFSET, session_id.get_string(), uuid::SIZE);
  string json = RP_TRY_RETURN(client.get_string(key), rp::to_unexpected);

  entity::User user{};

  // TODO: Integrate with Rapidjson
  json[json.get_size() - 2] = '\0';
  RP_TRY(
      user.set_username(
          json.c_str() + std::strlen(R"({"id":")") +
          std::strlen(R"(","username":")") + uuid::SIZE
      ),
      rp::to_unexpected
  );

  user.set_id(json.c_str() + std::strlen(R"({"id":")"));

  return std::move(user);
}

error_code SessionValkey::remove_session_impl(const uuid& session_id) noexcept {
  ValkeyClient client =
      RP_TRY_RETURN(this->manager->get_client(), rp::to_error_code);

  c8 key[PREFIX_OFFSET + uuid::SIZE + 1] = PREFIX; // NOLINT
  std::strncpy(key + PREFIX_OFFSET, session_id.get_string(), uuid::SIZE);

  return client.remove(key);
}

} // namespace repo
