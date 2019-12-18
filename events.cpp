#include "discord.hpp"
#include "gateway.hpp"
#include "bot.hpp"

#define EVENT_HANDLER(t) void gateway::event_##t(const discord::payload &msg)
#define GATEWAY_HANDLE(op) void gateway::handle_##op(const discord::payload &msg)

namespace backend {
  /**
   * @brief: handle the READY event
   *
   * The ready event provides the following information:
   *    1) sharding
   *    2) user object
   *    3) guilding
   *    4) session id
   */
  EVENT_HANDLER(READY) {
    session_id = msg.d["session_id"].get<std::string>();
    guild_info.id = msg.d["guilds"][0]["id"].get<std::string>(); // part of only 1 guild
    //user_info = parse_user(msg.d["user"]); TODO fix this
    *status = ACTIVE;
  }

  EVENT_HANDLER(RESUMED) {
    *up_to_date = true;
  }

  EVENT_HANDLER(RECONNECT) {
  }

  EVENT_HANDLER(HELLO){}
  EVENT_HANDLER(INVALID_SESSION){}
  EVENT_HANDLER(CHANNEL_CREATE){}
  EVENT_HANDLER(CHANNEL_UPDATE){}
  EVENT_HANDLER(CHANNEL_DELETE){}
  EVENT_HANDLER(CHANNEL_PINS_UPDATE){}

  /**
   * @brief: grab guild information
   *
   * This is a very important event because it contains information about the guild as well as
   * all of the users that exists insdie of it. Pearlbot only exists in one guild so only one, so it
   * holds the guild object instead of vector of guilds.
   */
  EVENT_HANDLER(GUILD_CREATE) {
    guild_info.member_count = msg.d["member_count"].get<int>();
    auto roles = msg.d["roles"];
    size_t idx = 0;
    while(!roles[idx++].is_null()) {
    }
  }

  EVENT_HANDLER(GUILD_UPDATE){}
  EVENT_HANDLER(GUILD_DELETE){}
  EVENT_HANDLER(GUILD_BAN_ADD){}
  EVENT_HANDLER(GUILD_BAN_REMOVE){}
  EVENT_HANDLER(GUILD_EMOJIS_UPDATE){}
  EVENT_HANDLER(GUILD_INTEGRATION_UPDATE){}
  EVENT_HANDLER(GUILD_MEMBER_ADD){}
  EVENT_HANDLER(GUILD_MEMBER_REMOVE){}
  EVENT_HANDLER(GUILD_MEMBER_UPDATE){}
  EVENT_HANDLER(GUILD_MEMBER_CHUNK){}
  EVENT_HANDLER(GUILD_ROLE_CREATE){}
  EVENT_HANDLER(GUILD_ROLE_UPDATE){}
  EVENT_HANDLER(GUILD_ROLE_DELETE){}

  EVENT_HANDLER(MESSAGE_CREATE) {
    std::string content = msg.d["content"].get<std::string>();
    if(content.at(0) == '#') {
      content.erase(0,1);
      if (content == "gaygang") {
        command_q->push(GAYGANG);
      }
    }
  }

  EVENT_HANDLER(MESSAGE_UPDATE){}
  EVENT_HANDLER(MESSAGE_DELETE){}
  EVENT_HANDLER(MESSAGE_DELETE_BULK){}
  EVENT_HANDLER(MESSAGE_REACTION_ADD){}
  EVENT_HANDLER(MESSAGE_REACTION_REMOVE){}
  EVENT_HANDLER(MESSAGE_REACTION_REMOVE_ALL){}

  EVENT_HANDLER(PRESENCE_UPDATE) {
  }

  EVENT_HANDLER(TYPING_START) {}
  EVENT_HANDLER(USER_UPDATE){}
  EVENT_HANDLER(VOICE_STATE_UPDATE){}
  EVENT_HANDLER(VOICE_SERVER_UPDATE){}
  EVENT_HANDLER(WEBHOOKS_UPDATE){}

  GATEWAY_HANDLE(DISPATCH) {
    last_sequence_data++;
    (this->*(this->events[msg.t]))(msg);
  }

  /**
   * @brief: handle a heartbeat request from wss
   *
   * Sends a heartbeat back.
   *
   * @bug: will send twice because other thread will send a heartbeat too
   */
  GATEWAY_HANDLE(HEARTBEAT) {
    send_payload(
        {
          { "op", discord::HEARTBEAT },
          { "d", this->last_sequence_data },
        });
  }

  GATEWAY_HANDLE(IDENTIFY) {}

  GATEWAY_HANDLE(STATUS_UPDATE) {}

  GATEWAY_HANDLE(VOICE_UPDATE) {}

  GATEWAY_HANDLE(RESUME) {}

  GATEWAY_HANDLE(RECONNECT) {}

  GATEWAY_HANDLE(REQUEST_GUILD_MEMBERS) {}

  GATEWAY_HANDLE(INVALID_SESS) {
    *status = DISCONNECTED;
    *up_to_date = false;
  }

  /**
   * @brief: responds to the HELLO payload
   *
   * The initial HELLO payload provides the heartbeat_interval as well as
   * the gateway server location.
   */
  GATEWAY_HANDLE(HELLO) {
    // this is the first time we've received a HELLO packet
    if(*status == NEW) {
      heartbeat_interval = msg.d["heartbeat_interval"].get<int>();
      // the first identify payload is unique
      send_payload(
          {
            {U("d"),{
              { U("token"), token },
              { U("properties"), {
                { U("$os"), U("linux") },
                { U("$browser"), U("Discord") },
                { U("$device"), U("Discord") }}
              },
              { U("compress"), false }}
            },
            { U("op"), discord::IDENTIFY },
          });
    } else if(*status == DISCONNECTED) {
      send_payload({
          { "token", token },
          { "session_id", session_id },
          { "seq", last_sequence_data }
        });
    }
  }

  GATEWAY_HANDLE(HEARTBEAT_ACK) {
    heartbeat_lock.lock();
    heartbeat_ticks--;
    heartbeat_lock.unlock();
  }
}
