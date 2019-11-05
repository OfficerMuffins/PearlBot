#include "discord.hpp"
#include "utils.hpp"
#include <cpprest/http_client.h>
#include <cpprest/ws_client.h>
#include <nlohmann/json.hpp>
#include <iostream>
#include <string>

namespace discord {
  payload::payload(opcodes op, int s, std::string t, nlohmann::json d) : op{op}, d{d}, s{s}, t{t} {;}
  payload::payload(opcodes op) { payload(op, 0, "null", {}); }

  Connection::Connection(encoding enc, bool compress, std::string uri, state state) :
    status{state}, encoding_type{enc}, compress{compress}, uri{uri} {;}

  Connection::Connection()
  {
    Connection(Connection::encoding::JSON, false, "", DEAD);
  }

  pplx::task<nlohmann::json> Connection::get_wss(const std::string &token)
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

  pplx::task<nlohmann::json> Connection::send_payload(const nlohmann::json &payload)
  /**
   * @brief: sends the payload via websocket
   */
  {
    websocket_outgoing_message out_msg;
    out_msg.set_utf8_message(payload.dump());
    std::cout << "Sending " << payload.dump() << std::endl;
    // before sending a payload, send empty message so that we can
    // connect to the websocket api
    // append json headers created from the payload
    client.send(out_msg).wait();
    return client.receive().then([](websocket_incoming_message msg) {
        std::string out = msg.extract_string().get();
        std::cout << "message: " << out << std::endl;
        auto parsed_json = out.empty() ? nlohmann::json{} : nlohmann::json::parse(out);
        return parsed_json;
      });
  }

  nlohmann::json Connection::package(const payload &payload)
  /**
   * @brief: packages the payload into a readable input for the web request
   */
  {
    nlohmann::json data;
    data =
    {
      {"op", payload.op},
      {"s", payload.s},
      {"t", payload.t}
    };
    switch(payload.op) {
      case(IDENTIFY):
        extendJson(data,
            { "d",
              {
                { "token", token },
                { "session_id", session_id },
                { "seq", last_sequence_data }
              }
            });
        break;
      default:
        extendJson(data, {"d" , nullptr});
    }
    return data;
  }

  void Connection::pulse()
  /**
   * @brief: sends a heartbeat payload
   */
  {
    while(status == state::ACTIVE) {
      std::this_thread::sleep_for(std::chrono::milliseconds{heartbeat - 5});
      auto resp = send_payload(package({HEARTBEAT})).get();
      if(resp["op"] == HEARTBEAT_ACK) {
        std::cout << "received an acknowledge!" << std::endl;
      }
    }
  }

  void Connection::end()
  {
    status = state::DEAD;
    /*
    for(std::vector<std::thread>::iterator it = threads.begin();
        it != threads.end();
        it++)
      it->join();*/
    heartbeat_thread.join();
    client.close().wait();
  }

  void Connection::handle_gateway(const std::string &token)
  {
    nlohmann::json dump;
    // should contain wss url, only expected field is the url field
    dump = get_wss(token).get();
    // FIXME hardcoded for now
    uri = dump.value("url", "\0") + "/?v=6&encoding=json";
  }

  void Connection::handshake(const std::string &token)
  {
    nlohmann::json dump;
    // send an empty message so that we can obtain a heartbeat interval
    client.connect(uri).then([this]() {
        std::cout << "Connected to " << this->uri << std::endl;
        }).wait();
    dump = send_payload(package({HELLO})).get();
    heartbeat = dump["d"]["heartbeat_interval"];
    // set the state of the connection so that we can start heartbeating
    status = state::ACTIVE;
    // now we begin communication with the gateway
    dump = send_payload(package({IDENTIFY, 0, "null",
          { "d",
            {
              { "token", token },
                { "properties",
                  {
                    { "$os", "linux" },
                    { "$browser", "PearlBot" },
                    { "$device", "PearlBot" }
                  }
                },
              { "compress", false },
              { "large_threshold", 250 }
            }
          }
        })).get();
    heartbeat_thread = std::thread{ &Connection::pulse, this };
    while(1);
  }
}
