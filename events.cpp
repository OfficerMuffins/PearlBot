#include "discord.hpp"

#define EVENT_HANDLER(t) void Connection::event_##t(const payload &msg)

namespace discord {
  EVENT_HANDLER(READY) {
    std::cout << "Ready event found" << std::endl;
    shard_id = msg.d["shard"][0].get<int>();
    num_shards = msg.d["shard"][1].get<int>();
    session_id = msg.d["session_id"].get<std::string>();
    guild_info.id = msg.d["guilds"]["id"].get<snowflake_t>();
    //guilds = msg.d["guilds"].get<std::vector>(); TODO
    user_info.id = msg.d["user"]["id"].get<int>();
    user_info.username = msg.d["user"]["username"].get<std::string>();
    status = ACTIVE;
  }

  EVENT_HANDLER(RESUMED) {
    up_to_date = true;
  }

  EVENT_HANDLER(RECONNECT) {
  }

  EVENT_HANDLER(HELLO){}
  EVENT_HANDLER(INVALID_SESSION){}
  EVENT_HANDLER(CHANNEL_CREATE){}
  EVENT_HANDLER(CHANNEL_UPDATE){}
  EVENT_HANDLER(CHANNEL_DELETE){}
  EVENT_HANDLER(CHANNEL_PINS_UPDATE){}
  EVENT_HANDLER(GUILD_CREATE){}
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
  EVENT_HANDLER(MESSAGE_CREATE){}
}
