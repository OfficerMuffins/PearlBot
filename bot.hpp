#pragma once
#include "discord.hpp"

enum state {
  ACTIVE,
  DISCONNECTED,
  NEW
};

class Bot {
  private:
    // shared between bot, gateway, and connection
    std::string token;
    state status;
    bool up_to_date;

  public:
    char ref;

    Bot(std::string, char);
    int run();
    state* get_state_ptr();
    bool* get_up_to_date_ptr();
};
