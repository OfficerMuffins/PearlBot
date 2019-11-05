#pragma once
#ifndef __DISCORD_HPP__
#define __DISCORD_HPP__

#include "bot.hpp"

#include <nlohmann/json.hpp>
#include <cpprest/ws_client.h>
#include <cpprest/http_client.h>
#include <string>
#include <cstdint>

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
      payload(opcodes op, std::unordered_map<std::string, std::string> d, int s = 0, std::string t = "null") : op{op}, d{d}, s{s}, t{t} {};
  } payload;

  class Bot {
    public:
      std::string token;
      char ref;

      Bot(std::string, char);
      Bot() : ref{'\0'}, token{std::string{"."}} {};
      int run();
      void login(const std::string &);
  };

  typedef web::websockets::client::websocket_client wss_client;
  typedef web::websockets::client::websocket_outgoing_message websocket_outgoing_message;
  typedef web::websockets::client::websocket_incoming_message websocket_incoming_message;
  typedef uint64_t snowflake_t;
  // should be obtained during HTTP gateway
  extern Bot pearlbot;
  extern int sharding;
  extern snowflake_t snowflake;
  extern wss_client client;
}
#endif
