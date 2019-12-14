#include "utils.hpp"
#include "discord.hpp"

namespace discord {
  payload::payload(opcodes op, nlohmann::json d, int s, std::string t) :
    op{op}, d{d}, s{s}, t{t} {;}

  /**
   * @brief: packages the payload into a readable input for the web request
   */
  nlohmann::json Connection::package(const payload payload) {
    nlohmann::json data =
    {
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
      case(IDENTIFY):
        data.update(
          {
            {"d", {
                { "token", token },
                { "session_id", session_id },
                { "seq", last_sequence_data }
              }
            }
          });
        break;
      default:
        data.update(payload.d);
    }
    return data;
  }

  payload Connection::unpack(const nlohmann::json msg) {
    return payload(
        static_cast<opcodes>(msg["op"].get<int>()),
        msg["d"],
        msg["s"].is_null() ? 0 : msg["s"].get<int>(),
        msg["t"].is_null() ? "" : msg["t"].get<std::string>());
  }
}
