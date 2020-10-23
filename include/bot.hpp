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
    state status;
    bool up_to_date;
    std::queue<bot_task> command_q;
    backend::gateway wss_gateway;
    backend::client c;
    std::string token;
    char ref;

  public:
    discord::guild guild_info;
    discord::role bot_role;

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
