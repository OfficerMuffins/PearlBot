#include "discord.hpp"

#define HANDLE_DEFINE(op) void Connection::handle_##op(const payload &msg)

namespace discord {
  /**
   * @brief: responds to the HELLO payload
   */
  HANDLE_DEFINE(HELLO) {
    heartbeat_interval = msg.d["heartbeat_interval"].get<int>();
    status = state::ACTIVE;
    // the first identify payload is unique
    send_payload(
        {
          {"d",{
            { "token", token },
            { "properties", {
              { "$os", "linux" },
              { "$browser", "Discord" },
              { "$device", "Discord" }}
            },
            { "compress", false }}
          },
          {"op", IDENTIFY},
        });
  }

  /**
   * @brief: handle the ready event
   *
   * The ready event is sent after the identify packet and gives us the following relevant
   * information: gateway protocol, information about user (including email), the guilds that
   * the user is in (aka the servers), session id, shard
   */
  HANDLE_DEFINE(IDENTIFY) {
    // extract the relevant information
    std::string guild_id = msg.d["guilds"]["id"].get<std::string>();
    session_id = msg.d["user"]["id"].get<std::string>();
  }

  HANDLE_DEFINE(HEARTBEAT_ACK) {
  }

  HANDLE_DEFINE(INVALID_SESS) {
  }
}
