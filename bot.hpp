#pragma once
#include "discord.hpp"
#include "connection.hpp"
#include "gateway.hpp"
#include "endpoints.hpp"

#include <queue>

class Bot {
  friend class backend::gateway;
  friend class backend::client;

  private:
    // shared between bot, gateway, and connection
    state status; // if the bot is active or not
    bool up_to_date; // used when the bot has disconnected and needs to retrace its steps
    std::queue<std::string> command_q; // issued to maintain commands
    backend::gateway wss_gateway; // gateway object, handles state
    backend::client c; // handles interactions with REST API
    std::string token; // secret token of the bot
    char ref; // issues command to bot
    discord::guild guild_info; // contains guild info, Pearlbot is only used for 1 guild

    std::vector<discord::user> whitelist;
    std::vector<discord::user> blacklist;

  public:
    Bot(uint64_t, std::string, char);
    int run();
    void create_message();
    void ping_user();
};

void command_gaygang(const Bot &);
