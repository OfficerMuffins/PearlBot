#pragma once
#include "connection.hpp"
#include <cpprest/http_client.h>
#include <cpprest/uri_builder.h>

namespace backend {
  typedef web::uri_builder uri_builder;
  static const std::string base_uri = "https://discordapp.com/api/v6/gateway";

  class client : public Connection {
    enum endpoints {
      CHANNEL,
      GUILDS,
      INVITES,
      USERS,
      VOICE,
      CHANNELS,
    };

    public:
      pplx::task<nlohmann::json> identify();
      void send_message(std::string);

    private:
      // utilities
      static uri_builder make_endpoint(endpoints);
  };
}
