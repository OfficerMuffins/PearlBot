#include "utils.hpp"
#include "discord.hpp"

namespace discord {
  payload::payload(opcodes op, nlohmann::json d, int s, std::string t) :
    op{op}, d{d}, s{s}, t{t} {;}

  /**
   * @brief: packages the payload into a readable input for the web request
   */
  nlohmann::json Connection::package(const payload payload) {
    nlohmann::json data =
    {
      {"op", payload.op}
    };
    // nlohmann::json expects nullptr to insert null
    if(payload.s != 0) {
      data.update( { {"s", payload.s} } );
    }
    if(!payload.t.empty()) {
      data.update( { {"t", payload.t} } );
    }
    switch(payload.op) {
      case(IDENTIFY):
        data.update(
            {
            {"d", {
            { "token", token },
            { "session_id", session_id },
            { "seq", last_sequence_data }
            }
            }
            });
        break;
      default:
        data.update(payload.d);
    }
    return data;
  }

  /**
   * @brief: turns raw json to payload object
   *
   * The json is directly taken from the incoming message of the wss and is turned
   * into a payload struct.
   *
   * @param[in]: nlohmann json object
   * @return: payload object with the respective fields
   * @bug: d can also be an integer
   */
  payload Connection::unpack(const nlohmann::json msg) {
    return payload(
        static_cast<opcodes>(msg["op"].get<int>()),
        msg["d"],
        msg["s"].is_null() ? 0 : msg["s"].get<int>(),
        msg["t"].is_null() ? "" : msg["t"].get<std::string>());
  }

  /**
   * @brief: grab wss url from discord HTTP response
   */
  pplx::task<nlohmann::json> Connection::get_wss() {
    // required headers the first request
    web::http::http_request request(web::http::methods::GET);
    // User agent fields are required for custom implemented APIs
    request.headers().add(U("Authorization"), utility::conversions::to_string_t("Bot " + token));
    request.headers().add(U("Content-Type"), U("application/json"));
    request.headers().add(U("User-Agent"), U("DiscordBot https://github.com/OfficerMuffins/Pearlbot 1.0.0"));
    // GET is the default method, but let's be explicit
    // params: base_uri, config
    web::http::client::http_client client{ {U("https://discordapp.com/api/v6/gateway")}};
    return client.request(request).then([](web::http::http_response response) -> nlohmann::json {
        int code = response.status_code();
        switch(code) {
          case(200): {
          // TODO should error if the json dump is empty
          std::string json_dump = response.extract_utf8string(true).get();
          auto parsed_json = json_dump.empty() ? nlohmann::json{} : nlohmann::json::parse(json_dump);
          return parsed_json;
        }
        default: {
          throw code;
        }
      }
    });
  }
}
