#include "discord.hpp"

#include <cpprest/http_client.h>
#include <cpprest/ws_client.h>
#include <nlohmann/json.hpp>
#include <iostream>


pplx::task<nlohmann::json> get_wss(const std::string &token)
/**
 * @brief: grab wss from discord HTTP response
 */
{
  // required headers the first request
  web::http::http_request request(web::http::methods::GET);
  // User agent fields are required for custom implemented APIs
  request.headers().add(U("Authorization"), utility::conversions::to_string_t("Bot " + token));
  request.headers().add(U("Content-Type"), U("application/json"));
  request.headers().add(U("User-Agent"), U("DiscordBot https://github.com/OfficerMuffins/Pearlbot 1.0.0"));
  // uri and endpoint
  std::cout << "Preparing to send empty request" << std::endl;
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
            discord::wss_url = utility::string{parsed_json.at("url")};
            return parsed_json;
        }
        default: {
            throw code;
        }
      }
    });
}

pplx::task<void> send_payload(const discord::payload &payload)
{
  using wss = web::websockets;
  auto packaged_json = package(payload);
  discord::wss_client.connect(wss_url).then([](){}).wait();
  wss::websocket_outgoing_message msg;
  std::cout << packaged_json.to_string() < std::endl;
  wss_client.send(msg);
  return wss_client.recieve().then([&](wss::websocket_incoming_message msg) {
      std::cout << msg.extract_string() std::endl;
      });
}

nlohmann::json package(const discord::payload &payload)
{
  return {
    {"op", payload.op},
    {"d", payload.d},
    {"s", payload.s},
    {"t", payload.t}
  };
}
