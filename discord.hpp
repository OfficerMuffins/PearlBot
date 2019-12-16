#pragma once
#include <string>
#include <nlohmann/json.hpp>

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

  enum state {
    ACTIVE,
    DISCONNECTED,
    NEW
  };

  enum encoding {
    JSON,
    ETF
  };

  struct user {
    std::string id;
    std::string username;
    std::string discriminator;
    std::string avatar;
    bool bot;
    bool system;
    bool mfa_enabled;
    std::string locale;
    bool verified;
    std::string email;
    int flags;
    int premium_type;
  };

  struct guild {
    std::string id;
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
