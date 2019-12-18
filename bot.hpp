#pragma once
#include "discord.hpp"
#include "connection.hpp"
#include "gateway.hpp"
#include "endpoints.hpp"

#include <queue>

class Bot {
  private:
    // shared between bot, gateway, and connection
    state status;
    bool up_to_date;
    std::shared_ptr<std::queue<commands>> command_q;
    backend::gateway wss_gateway;
    backend::client c;

  public:
    std::string token;
    char ref;
    std::shard_ptr<discord::guild> guild; // contains guild info

    Bot(std::string, char);
    int run();
    void create_message();
};
