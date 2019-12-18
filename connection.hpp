#pragma once
#include "discord.hpp"

#include <queue>
#include <memory>

namespace backend {
  enum encoding {
    JSON,
    ETF
  };

  class Connection {
    public:
      void init_shared(state*, bool*, std::string);
      void init_cmd_q(std::shared_ptr<std::queue<commands>> &);
    protected:
      // shared between bot, gateway, and connection
      state *status;
      bool *up_to_date;
      std::shared_ptr<std::queue<commands>> command_q;
      std::string token;
  };
}
