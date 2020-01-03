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
    std::queue<bot_task> command_q; // issued to maintain commands
    backend::gateway wss_gateway; // gateway object, handles state
    backend::client c; // handles interactions with REST API
    std::string token; // secret token of the bot
    char ref; // issues command to bot

  public:
    // internal server information
    discord::guild guild_info; // contains guild info, Pearlbot is only used for 1 guild
    discord::role bot_role; // role information for bot, primarily only used for permissions

    uint64_t curr_chan;

    Bot(std::string, char);
    Bot();
    ~Bot();
    int run();
    void create_message(std::string);
    void ping_user();

    std::vector<discord::member> whitelist;
    std::vector<discord::member> blacklist;
};
