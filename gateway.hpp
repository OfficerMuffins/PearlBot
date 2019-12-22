#pragma once
#include "discord.hpp"
#include "utils.hpp"
#include "connection.hpp"

#include <nlohmann/json.hpp>
#include <cpprest/ws_client.h>
#include <cpprest/http_client.h>
#include <boost/asio.hpp>
#include <string>
#include <thread>

#define HANDLE_DECL(op) void handle_##op(const discord::payload &)
#define HANDLE_ENTRY(op) { discord::op, &gateway::handle_##op }

#define EVENT_ENTRY(t) { #t, &gateway::event_##t }
#define EVENT_DECL(t) void event_##t(const discord::payload&)
#define NUM_THREADS 3

namespace backend {
  typedef web::websockets::client::websocket_callback_client wss_client;
  typedef web::websockets::client::websocket_outgoing_message websocket_outgoing_message;
  typedef web::websockets::client::websocket_incoming_message websocket_incoming_message;
  typedef web::websockets::client::websocket_close_status wss_close_status;
  typedef uint64_t snowflake_t;

  /**
   * @class: gateway class that manages the state of the connection
   *
   * The discord API communicates to the gateways via wss. This persistent connection
   * is used to maintain the states and activity of the guilds. The gateway class maintains
   * the handling of events and heartbeating.
   */
  class gateway : public Connection {
    const unsigned long rate_limit = 120;
    const std::string uri = "wss://gateway.discord.gg/?v=6&encoding=json";

    public:
      gateway(Bot *const, bool, encoding);
      void run(); // main

    private:
      // connection settings
      backend::encoding encoding_type;
      bool compress;
      wss_client client;
      std::mutex client_lock;

      // important information obtained from server, might be shared with other users
      int heartbeat_interval = 0;
      int num_shards = 0;
      int shard_id = 0;
      std::string session_id;
      int last_sequence_data = 0;

      // threads, manages work for the stateful gateway
      boost::asio::thread_pool workers;
      void manage_resources(); // resets the rate limit every 60 seconds
      void manage_events(); // manage sending of events and rate limiting
      void heartbeat(std::promise<void> &); // handle heartbeat

      // utilities
      nlohmann::json package(const discord::payload &payload);
      static discord::payload unpack(const nlohmann::json msg);
      void send_payload(const nlohmann::json&);

      // handlers
      void handle_callback(const websocket_incoming_message &); // wss client message handler
      void close(); // close connection and join threads
      void reconnect(); // attempt a reconnection by sending RESUME

      std::mutex heartbeat_lock; // heartbeat threads, keep the connection heartbeating
      int heartbeat_ticks = 0; // acts as a sort of binary semaphore

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

      std::unordered_map<discord::opcodes, void (gateway::*)(const discord::payload&)> handlers = {
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
      std::queue<discord::payload> event_q; // needed because events are rate limited
      std::mutex event_q_lock;
      semaphore rate_sem; // moderate the rate limit

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
      EVENT_DECL(MESSAGE_UPDATE);
      EVENT_DECL(MESSAGE_DELETE);
      EVENT_DECL(MESSAGE_DELETE_BULK);
      EVENT_DECL(MESSAGE_REACTION_ADD);
      EVENT_DECL(MESSAGE_REACTION_REMOVE);
      EVENT_DECL(MESSAGE_REACTION_REMOVE_ALL);
      EVENT_DECL(PRESENCE_UPDATE);
      EVENT_DECL(TYPING_START);
      EVENT_DECL(USER_UPDATE);
      EVENT_DECL(VOICE_STATE_UPDATE);
      EVENT_DECL(VOICE_SERVER_UPDATE);
      EVENT_DECL(WEBHOOKS_UPDATE);

      std::unordered_map<std::string, void (gateway::*)(const discord::payload&)> events = {
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
        EVENT_ENTRY(MESSAGE_UPDATE),
        EVENT_ENTRY(MESSAGE_DELETE),
        EVENT_ENTRY(MESSAGE_DELETE_BULK),
        EVENT_ENTRY(MESSAGE_REACTION_ADD),
        EVENT_ENTRY(MESSAGE_REACTION_REMOVE),
        EVENT_ENTRY(MESSAGE_REACTION_REMOVE_ALL),
        EVENT_ENTRY(PRESENCE_UPDATE),
        EVENT_ENTRY(TYPING_START),
        EVENT_ENTRY(USER_UPDATE),
        EVENT_ENTRY(VOICE_STATE_UPDATE),
        EVENT_ENTRY(VOICE_SERVER_UPDATE),
        EVENT_ENTRY(WEBHOOKS_UPDATE),
    };
  };
}
