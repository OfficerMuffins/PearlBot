#include "discord.hpp"
#include "utils.hpp"
#include <cpprest/http_client.h>
#include <cpprest/ws_client.h>
#include <nlohmann/json.hpp>
#include <iostream>

using namespace discord;

pplx::task<nlohmann::json>
get_wss(const std::string &token)
/**
 * @brief: grab wss url from discord HTTP response
 */
{
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

pplx::task<void>
send_payload(const payload &payload, std::string &wss_url)
/**
 * @brief: sends the payload via websocket
 */
{
  websocket_outgoing_message msg;
  auto packaged_json = package(payload);
  std::cout << packaged_json.dump() << std::endl;
  // before sending a payload, send empty message so that we can
  // connect to the websocket api
  // TODO make sure wss_url is correct encoding
  client.connect(wss_url).then([]() {
      std::cout << "finished connecting" << std::endl;
      });
  // append json headers created from the payload
  /*
  for(auto &e : package(payload)) {
    std::cout << e.first() << " " << e.second() << std::endl;
  }*/
  std::cout << package(payload).dump() << std::endl;
  client.send(msg);
  return client.receive().then([](websocket_incoming_message msg) {
      std::cout << msg.extract_string().get() << std::endl;
      });
}

nlohmann::json
package(const payload &payload)
/**
 * @brief: packages the payload into a readable input for the web request
 */
{
  return {
    {"op", payload.op},
    {"d", payload.d},
    {"s", payload.s},
    {"t", payload.t}
  };
}
