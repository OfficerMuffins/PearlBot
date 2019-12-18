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
  void Connection::init_shared(state* status, bool* up_to_date, std::string token) {
    // make shared ptr on statck allocated variables
    this->up_to_date = up_to_date;
    this->status = status;
    this->token = token;
  }

  void Connection::init_cmd_q(std::shared_ptr<std::queue<commands>> &q) {
    command_q = std::shared_ptr<std::queue<commands>>(q);
  }
}
