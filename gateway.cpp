#include "discord.hpp"

#define HANDLE_DEFINE(op) void Connection::handle_##op(const payload &msg)
#define EVENT_HANDLER(t) void Connection::handle_event_##t(const payload &msg)

namespace discord {
  HANDLE_DEFINE(DISPATCH) {
  }

  /**
   * @brief: handle a heartbeat request from wss
   *
   * Sends a heartbeat back.
   *
   * @bug: will send twice because other thread will send a heartbeat too
   */
  HANDLE_DEFINE(HEARTBEAT) {
    send_payload(
        {
          { "op", HEARTBEAT },
          { "d", this->last_sequence_data },
        });
  }

  /**
   * @brief: handle the ready event
   *
   * The ready event is sent after the identify packet and gives us the following relevant
   * information: gateway protocol, information about user (including email), the guilds that
   * the user is in (aka the servers), session id, shard
   *
   * TODO clients are limited to 1 identify every 5 seconds
   */
  HANDLE_DEFINE(IDENTIFY) {
  }

  HANDLE_DEFINE(STATUS_UPDATE) {
  }

  HANDLE_DEFINE(VOICE_UPDATE) {
  }

  HANDLE_DEFINE(RESUME) {
  }

  HANDLE_DEFINE(RECONNECT) {
  }

  HANDLE_DEFINE(REQUEST_GUILD_MEMBERS) {
  }

  HANDLE_DEFINE(INVALID_SESS) {
  }

  /**
   * @brief: responds to the HELLO payload
   *
   * The initial HELLO payload provides the heartbeat_interval as well as
   * the gateway server location.
   */
  HANDLE_DEFINE(HELLO) {
    // this is the first time we've received a HELLO packet
    if(status == NEW) {
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
    } else if(status == DISCONNECTED) {
      send_payload({
          { "token", token },
          { "session_id", session_id },
          { "seq", last_sequence_data }
        });
    }
  }

  HANDLE_DEFINE(HEARTBEAT_ACK) {
    heartbeat_lock.lock();
    heartbeat_ticks--;
    heartbeat_lock.unlock();
  }
}
