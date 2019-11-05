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

pplx::task<nlohmann::json>
send_payload(const payload &payload)
/**
 * @brief: sends the payload via websocket
 */
{
  wss_client client;
  websocket_outgoing_message out_msg;
  auto payload_json = package(payload);
  out_msg.set_utf8_message(payload_json.dump());
  std::cout << payload_json.dump() << std::endl;
  // before sending a payload, send empty message so that we can
  // connect to the websocket api
  // TODO make sure wss_url is correct encoding
  wss_client.connect(uri).then([]() {
      std::cout << "Connected with the websocket" << std::endl;
      }).wait();
  // append json headers created from the payload
  wss_client.send(out_msg).wait();
  return wss_client.receive().then([](websocket_incoming_message msg) {
      std::string out = msg.extract_string().get();
      std::cout << "message: " << out << std::endl;
      auto parsed_json = out.empty() ? nlohmann::json{} : nlohmann::json::parse(out);
      return parsed_json;
    });
}

nlohmann::json
package(const payload &payload)
/**
 * @brief: packages the payload into a readable input for the web request
 */
{
  return {
    {"t", payload.t}
    {"s", payload.s},
    {"op", payload.op},
    {"d", payload.d},
  };
}

void
pulse()
/**
 * @brief: sends a heartbeat payload
 */
{
  auto resp = send_payload(payload{
          IDENTIFY,
          std::unordered_map<std::string, std::string> {
            {U("token"), this->token},
            {U("properties"), "{$os : linux, $browser: firefox, $device: laptop}"}
          }}
          ,wss_url).get();
  if(stoi(resp["op"]) == HEARTBEAT_ACK) {
    std::cout << "we received an acknowledge!" << std::endl;
  }
}

void
end()
{
  client.close().wait();
}
