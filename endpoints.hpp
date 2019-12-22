#pragma once
#include "connection.hpp"
#include <cpprest/http_client.h>
#include <cpprest/uri_builder.h>

namespace backend {
  typedef web::http::client::http_client http_client;
  typedef web::http::http_request http_request;
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

    discord::guild chan;

    public:
      pplx::task<nlohmann::json> identify();
      void send_message(std::string);
      client(Bot *const);

    private:
      // utilities
      static uri_builder make_endpoint(endpoints);
      void default_headers(http_request &);
  };
}
