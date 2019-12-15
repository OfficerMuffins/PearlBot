#pragma once
#ifndef __DISCORD_HPP__
#define __DISCORD_HPP__

#include "bot.hpp"
#include "utils.hpp"

#include <nlohmann/json.hpp>
#include <cpprest/ws_client.h>
#include <cpprest/http_client.h>
#include <string>
#include <cstdint>
#include <thread>
#include <future>

#define HANDLE_DECL(op) void handle_##op(const payload &)
#define HANDLE_ENTRY(op) { op, &Connection::handle_##op }

#define EVENT_ENTRY(t) { #t, &Connection::event_##t }
#define EVENT_DECL(t) void event_##t(const payload&)

namespace discord
{
  typedef web::websockets::client::websocket_callback_client wss_client;
  typedef web::websockets::client::websocket_outgoing_message websocket_outgoing_message;
  typedef web::websockets::client::websocket_incoming_message websocket_incoming_message;
  typedef web::websockets::client::websocket_close_status wss_close_status;
  typedef uint64_t snowflake_t;

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

  class payload {
    friend class Connection;

    public:
      opcodes op;
      nlohmann::json d;
      // last 2 are only used for opcode 10
      int s;
      std::string t;

      payload(opcodes op, nlohmann::json = {{"d", nullptr}}, int = 0, std::string = "");
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

  class Connection {
    friend class Bot;

    struct user {
      snowflake_t id;
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
      snowflake_t id;
    };

    private:
      // state variables
      state status;
      bool up_to_date;

      // connection settings
      encoding encoding_type;
      std::string uri;
      bool compress; // only supports zlib stream for now
      wss_client client;

      // important information obtained from server
      int heartbeat_interval;
      int num_shards;
      int shard_id;
      int heartbeat_ticks;
      user user_info;
      std::string session_id;
      int last_sequence_data;
      std::string token;
      guild guild_info;

      pplx::task<nlohmann::json> get_wss();
      void send_payload(const nlohmann::json&);

      explicit Connection(bool, state = DISCONNECTED, encoding = JSON);
      Connection();

      // utilities
      std::thread resource_manager; // just manages rate limit for now
      nlohmann::json package(const payload payload);
      static payload unpack(const nlohmann::json msg);

      // handlers
      void handle_callback(const websocket_incoming_message &);
      void close();
      void heartbeat(std::promise<void> &); // handle heartbeat
      void reconnect();
      void run();

      // heartbeat threads
      std::thread heartbeat_thread;
      std::mutex heartbeat_lock;

      HANDLE_DECL(DISPATCH);
      HANDLE_DECL(HEARTBEAT);
      HANDLE_DECL(IDENTIFY);
      HANDLE_DECL(STATUS_UPDATE);
      HANDLE_DECL(VOICE_UPDATE);
      HANDLE_DECL(RESUME);
      HANDLE_DECL(RECONNECT);
      HANDLE_DECL(REQUEST_GUILD_MEMBERS);
      HANDLE_DECL(INVALID_SESS);
      HANDLE_DECL(HELLO);
      HANDLE_DECL(HEARTBEAT_ACK);

      std::unordered_map<opcodes, void (Connection::*)(const payload&)> handlers = {
        HANDLE_ENTRY(DISPATCH),
        HANDLE_ENTRY(HEARTBEAT),
        HANDLE_ENTRY(IDENTIFY),
        HANDLE_ENTRY(STATUS_UPDATE),
        HANDLE_ENTRY(VOICE_UPDATE),
        HANDLE_ENTRY(RESUME),
        HANDLE_ENTRY(RECONNECT),
        HANDLE_ENTRY(REQUEST_GUILD_MEMBERS),
        HANDLE_ENTRY(INVALID_SESS),
        HANDLE_ENTRY(HELLO),
        HANDLE_ENTRY(HEARTBEAT_ACK),
      };

      // event stuff
      void manage_events();
      std::thread event_thread;
      std::queue<payload> event_q; // needed because events are rate limited
      std::mutex event_q_lock;
      int ticks; // counts # of events sent, reset every 60 seconds
      static const unsigned long rate_limit = 120;
      semaphore rate_sem{ rate_limit }; // moderate the rate limit
      EVENT_DECL(HELLO);
      EVENT_DECL(READY);
      EVENT_DECL(RESUMED);
      EVENT_DECL(RECONNECT);
      EVENT_DECL(INVALID_SESSION);
      EVENT_DECL(CHANNEL_CREATE);
      EVENT_DECL(CHANNEL_UPDATE);
      EVENT_DECL(CHANNEL_DELETE);
      EVENT_DECL(CHANNEL_PINS_UPDATE);
      EVENT_DECL(GUILD_CREATE);
      EVENT_DECL(GUILD_UPDATE);
      EVENT_DECL(GUILD_DELETE);
      EVENT_DECL(GUILD_BAN_ADD);
      EVENT_DECL(GUILD_BAN_REMOVE);
      EVENT_DECL(GUILD_EMOJIS_UPDATE);
      EVENT_DECL(GUILD_INTEGRATION_UPDATE);
      EVENT_DECL(GUILD_MEMBER_ADD);
      EVENT_DECL(GUILD_MEMBER_REMOVE);
      EVENT_DECL(GUILD_MEMBER_UPDATE);
      EVENT_DECL(GUILD_MEMBER_CHUNK);
      EVENT_DECL(GUILD_ROLE_CREATE);
      EVENT_DECL(GUILD_ROLE_UPDATE);
      EVENT_DECL(GUILD_ROLE_DELETE);
      EVENT_DECL(MESSAGE_CREATE);

      std::unordered_map<std::string, void (Connection::*)(const payload&)> events = {
        EVENT_ENTRY(HELLO),
        EVENT_ENTRY(READY),
        EVENT_ENTRY(RESUMED),
        EVENT_ENTRY(RECONNECT),
        EVENT_ENTRY(INVALID_SESSION),
        EVENT_ENTRY(CHANNEL_CREATE),
        EVENT_ENTRY(CHANNEL_UPDATE),
        EVENT_ENTRY(CHANNEL_DELETE),
        EVENT_ENTRY(CHANNEL_PINS_UPDATE),
        EVENT_ENTRY(GUILD_CREATE),
        EVENT_ENTRY(GUILD_UPDATE),
        EVENT_ENTRY(GUILD_DELETE),
        EVENT_ENTRY(GUILD_BAN_ADD),
        EVENT_ENTRY(GUILD_BAN_REMOVE),
        EVENT_ENTRY(GUILD_EMOJIS_UPDATE),
        EVENT_ENTRY(GUILD_INTEGRATION_UPDATE),
        EVENT_ENTRY(GUILD_MEMBER_ADD),
        EVENT_ENTRY(GUILD_MEMBER_REMOVE),
        EVENT_ENTRY(GUILD_MEMBER_UPDATE),
        EVENT_ENTRY(GUILD_MEMBER_CHUNK),
        EVENT_ENTRY(GUILD_ROLE_CREATE),
        EVENT_ENTRY(GUILD_ROLE_UPDATE),
        EVENT_ENTRY(GUILD_ROLE_DELETE),
        EVENT_ENTRY(MESSAGE_CREATE),
      };
  };

  class Bot {
    private:
      std::string token;
      char ref;
      Connection connection;

    public:
      Bot(std::string, char);
      int run();
  };

  // should be obtained during HTTP gateway
  extern Bot pearlbot;
}
#endif
