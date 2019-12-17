#include "connection.hpp"

namespace backend {
  class endpoints : protected Connection {
    public:
      endpoints(Bot &bot);
  };
}
