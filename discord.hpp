#pragma once
#ifndef __DISCORD_HPP__
#define __DISCORD_HPP__

#include "bot.hpp"

#include <nlohmann/json.hpp>
#include <cpprest/ws_client.h>
#include <cpprest/http_client.h>
#include <string>
#include <cstdint>
#include <thread>

namespace discord
{

  typedef web::websockets::client::websocket_client wss_client;
  typedef web::websockets::client::websocket_outgoing_message websocket_outgoing_message;
  typedef web::websockets::client::websocket_incoming_message websocket_incoming_message;
  typedef uint64_t snowflake_t;

  typedef enum opcodes {
    DISPATCH = 0,
    HEARTBEAT = 1,
    IDENTIFY = 2,
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
    nlohmann::json d;
    // last 2 are only used for opcode 10
    int s;
    std::string t;

    public:
      payload(opcodes op, int, std::string, nlohmann::json);
      payload(opcodes op);
  } payload;

  class Connection {
    typedef enum state {
      ACTIVE,
      SLEEP,
      DEAD
    } state;

    typedef enum encoding {
      JSON,
      ETF
    } encoding;

    private:
      state status;
      encoding encoding_type;
      std::string uri;
      wss_client client;
      std::mutex buffer_lock;

      int heartbeat;
      int sharding;
      std::thread heartbeat_thread;
      snowflake_t snowflake;
      bool compress; // only supports zlib stream for now
      std::string token;
      int session_id;
      int last_sequence_data;

    public:
      Connection(encoding, bool, std::string, state);
      Connection();
      pplx::task<nlohmann::json> get_wss(const std::string &);
      nlohmann::json package(const payload&);
      pplx::task<nlohmann::json> send_payload(const nlohmann::json &);
      void end();
      void pulse();
      void handle_gateway(const std::string&);
      void handshake(const std::string &);
  };

  class Bot {
    friend class connection;

    private:
      std::string token;
      char ref;
      Connection connection;

    public:
      Bot(std::string, char);
      Bot();
      int run();
      void login();
  };

  // should be obtained during HTTP gateway
  extern Bot pearlbot;
}
#endif
