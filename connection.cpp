#include "connection.hpp"

namespace backend {
  /**
   * @brief: constructor for base class connection
   *
   * @param[in]: reference to bot that create the connection
   */
  Connection::Connection(Bot* const bot) : bot{bot} {}
}
