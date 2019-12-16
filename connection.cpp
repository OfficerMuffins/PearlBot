#include "connection.hpp"

namespace backend {
  Connection::Connection(Bot &bot) {
    // make shared ptr on statck allocated variables
    up_to_date = { bot.get_up_to_date_ptr(), [](bool*){} };
    status = { bot.get_state_ptr(), [](state*){} };
  }
}
