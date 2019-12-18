#include "discord.hpp"
#include "utils.hpp"
#include "connection.hpp"
#include "gateway.hpp"

#include <nlohmann/json.hpp>

namespace backend {
  /**
   * @brief: packages the payload into a readable input for the web request
   */
  nlohmann::json gateway::package(const discord::payload &payload) {
    nlohmann::json data = {
      {"op", payload.op}
    };
    // nlohmann::json expects nullptr to insert null
    if(payload.s != 0) {
      data.update( { {"s", payload.s} } );
    }
    if(!payload.t.empty()) {
      data.update( { {"t", payload.t} } );
    }
    switch(payload.op) {
      case(discord::RESUME):
        data.update(
          {
            {"d", {
              { "token", token },
              { "session_id", session_id },
              { "seq", last_sequence_data }}
            }
          });
        break;
      default:
        data.update(payload.d);
    }
    return data;
  }

  /**
   * @brief: turns raw json to payload object
   *
   * The json is directly taken from the incoming message of the wss and is turned
   * into a payload struct.
   *
   * @param[in]: nlohmann json object
   * @return: payload object with the respective fields
   * @bug: d can also be an integer
   */
  discord::payload gateway::unpack(const nlohmann::json msg) {
    return {
        static_cast<discord::opcodes>(msg["op"].get<int>()),
        msg["d"],
        msg["s"].is_null() ? 0 : msg["s"].get<int>(),
        msg["t"].is_null() ? "" : msg["t"].get<std::string>()};
  }

  discord::user parse_user(const nlohmann::json &user_obj) {
    discord::user user_info;
    user_info.avatar = user_obj["avatar"].is_null() ? "" : user_obj["avatar"].get<std::string>();
    user_info.bot = user_obj["bot"].get<bool>();
    user_info.discriminator = user_obj["discriminator"].get<std::string>();
    user_info.id = user_obj["id"].get<std::string>();
    user_info.username = user_obj["username"].get<std::string>();
    return user_info;
  }
}

namespace discord {
  payload::payload(opcodes op, nlohmann::json d, int s, std::string t) :
    op{op}, d{d}, s{s}, t{t} {;}
}
