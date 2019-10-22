#include "bot.hpp"

#include <nlohmann/json.hpp>
#include <cpprest/http_client.h>
#include <string>
#include <cstdint>

#pragma once
#ifndef __DISCORD_HPP__
#define __DISCORD_HPP__

namespace discord
{
  typedef enum opcodes {
    DISPATCH = 0,
    HEARTBEAT = 1,
    IDENTITY = 2,
    STATUS_UPDATE = 3,
    VOICE_UPDATE = 4,
    RESUME = 5,
    RECONNECT = 6,
    REQUEST_GUILD_MEMBERS = 7,
    INVALID_SESS = 8,
    HELLO = 10,
    HEARTBEAT_ACK = 11
  } opcodes;

  typedef struct payload {
    opcodes op;
    std::unordered_map<std::string, std::string> d;
    // last 2 are only used for opcode 10
    int s;
    std::string t;

    public:
      payload(opcodes op, std::unordered_map<std::string, std::string> d, int s = 0, std::string t = "null"): op{op}, d{d}, s{s}, t{t};
  } payload;

  // should be obtained during HTTP gateway
  extern web::websockets::client wss_client;
  extern int sharding;
  extern uint64_t snowflake;
}

pplx::task<nlohmann::json> get_wss(const std::string &);
nlohmann::json package(const discord::payload&);
pplx::task<void> send_payload(const discord::payload &payload)
#endif
