#include "discord.hpp"
#include "connection.hpp"
#include "endpoints.hpp"
#include "bot.hpp"

namespace backend {
  client::client(Bot *const bot) : Connection(bot) {}

  /**
   * @brief: grab wss url from discord HTTP response
   */
  pplx::task<nlohmann::json> client::identify() {
    http_request request(web::http::methods::GET);
    default_headers(request);
    // GET is the default method, but let's be explicit
    // params: base_uri, config
    web::http::client::http_client client{ {U(base_uri)}};
    return client.request(request).then([this](web::http::http_response response) -> nlohmann::json {
        int code = response.status_code();
        if(code != 200) {
          this->identify();
          return {};
        }
        std::string json_dump = response.extract_utf8string(true).get();
        std::cout << json_dump << std::endl;
        auto parsed_json = json_dump.empty() ? nlohmann::json{} : nlohmann::json::parse(json_dump);
        return parsed_json;
      }
    );
  }

  uri_builder client::make_endpoint(endpoints base) {
    switch(base) {
      case(CHANNELS):
        break;
      case(GUILDS):
        break;
      case(VOICE):
        break;
      case(INVITES):
        break;
      case(CHANNEL):
        break;
      case(USERS):
        break;
    };
    return { base_uri };
  }

  void client::create_message(std::string msg) {
    http_request request(web::http::methods::POST);
    default_headers(request);

    uri_builder endpoint(base_uri);

    nlohmann::json body {
        {"content", msg},
        {"tts", false}
    };

    endpoint.append_path("channels");

    // finds the first text channel. Only uses 1 text channel for now
    /*
    for(int i = 0; i < bot->guild_info.channels.size(); i++) {
      if(bot->guild_info.channels[i].type == 0) {
        chan = bot->guild_info.channels[i];
      }
    }*/
    endpoint.append_path(std::to_string(bot->curr_chan)); // FIXME
    endpoint.append_path("messages");

    request.set_body(body.dump(4));
    http_client client{ endpoint.to_uri() };
    client.request(request).then([](web::http::http_response response) {});
  }

  void client::default_headers(http_request &request) {
    // User agent fields are required for custom implemented APIs
    request.headers().add(U("Authorization"), utility::conversions::to_string_t("Bot " + bot->token));
    request.headers().add(U("Content-Type"), U("application/json"));
    request.headers().add(U("User-Agent"), U("DiscordBot https://github.com/OfficerMuffins/Pearlbot 1.0.0"));
  }
}
