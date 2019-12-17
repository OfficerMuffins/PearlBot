#include "connection.hpp"

namespace backend {
  /**
   * @brief: constructor for base class connection
   *
   * The constructor creates a shared pointer out of the up_to_date and status
   * variable that the bot object owns. These variables are changed and managed
   * throughout all of the gateway threads and bot. Operations on these variables must
   * be very careful and atomic.
   *
   * @param[in]: reference to bot that create the connection
   */
  Connection::Connection(Bot &bot) {
    // make shared ptr on statck allocated variables
    up_to_date = { bot.get_up_to_date_ptr(), [](bool*){} };
    status = { bot.get_state_ptr(), [](state*){} };
    token = bot.token;
  }
}
