#pragma once
#include "discord.hpp"
#include "bot.hpp"
#include <memory>

namespace backend {
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
