#pragma once
#include <vector>
#include <string>
#include <nlohmann/json.hpp>

class Bot;
class Connection;
class gateway;
class client;

enum state {
  ACTIVE,
  DISCONNECTED,
  NEW,
  TERMINATE
};

namespace discord {
  enum opcodes {
    DISPATCH = 0,
    HEARTBEAT = 1,
    IDENTIFY = 2,
    STATUS_UPDATE = 3,
    VOICE_UPDATE = 4,
    RESUME = 6,
    RECONNECT = 7,
    REQUEST_GUILD_MEMBERS = 8,
    INVALID_SESS = 9,
    HELLO = 10,
    HEARTBEAT_ACK = 11
  };

  namespace gateway {
    enum events {
      HELLO,
      READY,
      RESUMED,
      RECONNECT,
      INVALID_SESSION,
      CHANNEL_CREATE,
      CHANNEL_UPDATE,
      CHANNEL_DELETE,
      CHANNEL_PINS_UPDATE,
      GUILD_CREATE,
      GUILD_UPDATE,
      GUILD_DELETE,
      GUILD_BAN_ADD,
      GUILD_BAN_REMOVE,
      GUILD_EMOJIS_UPDATE,
      GUILD_INTEGRATION_UPDATE,
      GUILD_MEMBER_ADD,
      GUILD_MEMBER_REMOVE,
      GUILD_MEMBER_UPDATE,
      GUILD_MEMBER_CHUNK,
      GUILD_ROLE_CREATE,
      GUILD_ROLE_UPDATE,
      GUILD_ROLE_DELETE,
      MESSAGE_CREATE,
      MESSAGE_UPDATE,
      MESSAGE_DELETE,
      MESSAGE_DELETE_BULK,
      MESSAGE_REACTION_ADD,
      MESSAGE_REACTION_REMOVE,
      MESSAGE_REACTION_REMOVE_ALL,
      PRESENCE_UPDATE,
      TYPING_START,
      USER_UPDATE,
      VOICE_STATE_UPDATE,
      VOICE_SERVER_UPDATE,
      WEBHOOKS_UPDATE,

      // commands
      IDENTIFY,
      RESUME,
      HEARTBEAT,
      REQUEST_GUILD_MEMBERS,
      UPDATE_VOICE_STATUS,
      UPDATE_STATUS,
    };
  };

  struct user {
    uint64_t id;
    std::string username;
    uint64_t discriminator;
    uint64_t avatar;

    user(uint64_t);
    user();
    friend std::ostream& operator<<(std::ostream&, const user&);
    inline bool operator==(const user& other) { return other.id == this->id; }
    inline bool operator!=(const user& other) { return other.id != this->id; }
  };

  struct member {
    bool deaf;
    bool mute;
    std::string nick;
    user usr_info;

    member(uint64_t);
    member();
    inline bool operator==(const member& other) { return other.usr_info.id == this->usr_info.id; }
    inline bool operator!=(const member& other) { return other.usr_info.id != this->usr_info.id; }
  };

  struct role {
    std::string name;
    uint64_t id;
    bool managed;
    bool mentionable;
    uint64_t permissions;
    short position;
    bool hoist;
  };

  struct channel {
    int bitrate;
    std::string name;
    uint64_t id;
    short type;
    short position;
    short user_limit;
  };

  struct guild {
    std::string region;
    int member_count;
    uint64_t id;
    std::unordered_map<uint64_t, member> members;
    std::vector<role> roles;
    std::vector<channel> channels;
  };

  class payload {
    public:
      opcodes op;
      nlohmann::json d;
      // last 2 are only used for opcode 10
      int s;
      std::string t;

      payload(opcodes op, nlohmann::json = {{"d", nullptr}}, int = 0, std::string = "");
  };
}
