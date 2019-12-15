#include "discord.hpp"

#define EVENT_HANDLER(t) void Connection::event_##t(const payload &msg)

namespace discord {
  EVENT_HANDLER(READY) {
    shard_id = msg.d["shard"][0].get<int>();
    num_shards = msg.d["shard"][1].get<int>();
    session_id = msg.d["session_id"].get<std::string>();
    guild_info.id = msg.d["guilds"]["id"].get<snowflake_t>();
    //guilds = msg.d["guilds"].get<std::vector>(); TODO
    user_info.id = msg.d["user"]["id"].get<int>();
    user_info.username = msg.d["user"]["username"].get<std::string>();
  }

  EVENT_HANDLER(RESUMED) {
    up_to_date = true;
  }

  EVENT_HANDLER(RECONNECT) {
  }
}
