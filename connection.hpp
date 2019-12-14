#pragma once

namespace discord {
  class Connection {
    struct guild {
      std::string id;
      bool unavailable;
    };

    friend class Bot;

    private:
      // important set up information when communicating with the wss
      state status;
      encoding encoding_type;
      std::string uri;
      bool compress; // only supports zlib stream for now
      wss_client client;

      // important information obtained from server
      int heartbeat_interval;
      int num_shards;
      int shard_id;
      int heartbeat_ticks;

      std::string session_id;
      snowflake_t snowflake;

      int last_sequence_data;
      std::string token;

      pplx::task<nlohmann::json> get_wss();
      void send_payload(const nlohmann::json&);

      explicit Connection(bool, state = DEAD, encoding = JSON);
      explicit Connection();

      // utilities
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

      // handles out sending of events at the rate limit
      std::thread event_thread;
      std::mutex client_lock;

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
  };

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

  std::unordered_map<std::string, void (Connection::*)(const payload&) handlers = {
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
  }
}
