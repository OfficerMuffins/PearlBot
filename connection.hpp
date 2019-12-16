#pragma once
#include "discord.hpp"
#include "bot.hpp"
#include <memory>

namespace backend {
  static const unsigned long rate_limit = 120;
  static const std::string uri = "wss://gateway.discord.gg/?v=6&encoding=json";

  enum encoding {
    JSON,
    ETF
  };

  class Connection {
    protected:
      // shared between bot, gateway, and connection
      std::string token;
      std::shared_ptr<state> status;
      std::shared_ptr<bool> up_to_date;

    public:
      Connection(Bot &);
  };
}
