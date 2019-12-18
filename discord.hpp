#pragma once
#include <string>
#include <nlohmann/json.hpp>

enum state {
  ACTIVE,
  DISCONNECTED,
  NEW
};

enum commands {
  GAYGANG,
  LOOKUP_LEAGUE,
};

namespace discord {
  enum opcodes {
    DISPATCH = 0,
    HEARTBEAT = 1,
    IDENTIFY = 2,
    STATUS_UPDATE = 3,
    VOICE_UPDATE = 4,
    RESUME = 5,
    RECONNECT = 6,
    REQUEST_GUILD_MEMBERS = 7,
    INVALID_SESS = 9,
    HELLO = 10,
    HEARTBEAT_ACK = 11
  };

  struct user {
    std::string id;
    std::string username;
    std::string discriminator;
    std::string avatar;
    bool bot;
    bool deaf;
    bool mute;
  };

  struct role {
    std::string name;
    std::string id;
    bool managed;
    bool mentionable;
    unsigned long permissions;
    short position;
    bool hoist;
  };

  struct guild {
    int member_count;
    std::string id;
    std::vector<user> members;
    std::vector<role> roles;
  };

  struct channel {
    std::string name;
    std::string id;
    int type;
  };

  class payload {
    public:
      opcodes op;
      nlohmann::json d;
      // last 2 are only used for opcode 10
      int s;
      std::string t;

      payload(opcodes op, nlohmann::json = {{"d", nullptr}}, int = 0, std::string = "");
      payload(opcodes op);
  };
}
