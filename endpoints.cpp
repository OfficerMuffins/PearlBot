#include "endpoints.hpp"

namespace backend {
  /**
   * @brief: grab wss url from discord HTTP response
   */
  pplx::task<nlohmann::json> client::identify() {
    // required headers the first request
    web::http::http_request request(web::http::methods::GET);
    // User agent fields are required for custom implemented APIs
    request.headers().add(U("Authorization"), utility::conversions::to_string_t("Bot " + token));
    request.headers().add(U("Content-Type"), U("application/json"));
    request.headers().add(U("User-Agent"), U("DiscordBot https://github.com/OfficerMuffins/Pearlbot 1.0.0"));
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
}
