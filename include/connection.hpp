#pragma once
#include "discord.hpp"

namespace backend {
  enum encoding {
    JSON,
    ETF
  };

  class Connection {
    public:
      Connection(Bot *const);
    protected:
      // shared between bot, gateway, and connection
      Bot *const bot;
  };
}
