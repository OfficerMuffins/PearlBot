#include "discord.hpp"
#include "utils.hpp"
#include <cpprest/http_client.h>
#include <cpprest/ws_client.h>
#include <nlohmann/json.hpp>
#include <iostream>
#include <string>

namespace discord {
  Connection::Connection(bool compress, state status, encoding enc)
    : status{status}, encoding_type{enc}, compress{compress} {;}

  Connection::Connection() { Connection(false); }

  void Connection::set_token(std::string token) { this->token = token; };


  pplx::task<nlohmann::json> Connection::get_wss()
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
    client_lock.lock();
    websocket_outgoing_message out_msg;
    out_msg.set_utf8_message(payload.dump());
    // before sending a payload, send empty message so that we can
    // connect to the websocket api
    // append json headers created from the payload
    client.send(out_msg).wait();
    return client.receive().then([this](websocket_incoming_message msg) {
        this->client_lock.unlock();
        std::string out = msg.extract_string().get();
        auto parsed_json = out.empty() ? nlohmann::json{} : nlohmann::json::parse(out);
        std::cout << "message: " << parsed_json.dump(4) << std::endl;
        return parsed_json;
      });
  }

  void Connection::pulse()
  /**
   * @brief: sends a heartbeat payload
   */
  {
    while(status == state::ACTIVE) {
      auto resp = send_payload(package({HEARTBEAT})).get();
      if(resp["op"] == HEARTBEAT_ACK) {
        std::cout << "received an acknowledge!" << std::endl;
      }
      std::this_thread::sleep_for(std::chrono::milliseconds{heartbeat - 5});
    }
  }

  void Connection::end()
  /**
   * @brief: ends the connection with the websocket
   *
   * Declares the state dead which forces heartbeating to stop and join the main thread.
   * Once all threads have been joined, the client closes.
   */
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

  void Connection::handle_gateway()
  {
    nlohmann::json dump;
    // should contain wss url, only expected field is the url field
    dump = get_wss().get();
    // FIXME hardcoded for now
    uri = dump.value("url", "\0") + "/?v=6&encoding=json";
  }

  void Connection::handshake()
  {
    nlohmann::json dump;
    // send an empty message so that we can obtain a heartbeat interval
    client.connect(uri).then([this]() {
        std::cout << "Connected to " << this->uri << std::endl;
        }).wait();
    dump = send_payload(package({HELLO})).get();
    heartbeat = dump["d"]["heartbeat_interval"].get<int>();
    // the first identify payload is unique
    status = state::ACTIVE;
    heartbeat_thread = std::thread{ &Connection::pulse, this };
    std::this_thread::sleep_for(std::chrono::seconds{3});
    std::cout << "sending IDENTIFY" << std::endl;
    dump = send_payload(
        {
          {"d",{
            { "token", token },
            { "properties", {
              { "$os", "linux" },
              { "$browser", "PearlBot" },
              { "$device", "PearlBot" }}
            },
            { "compress", false }}
          },
          {"op", IDENTIFY},
        }).get();
  }

  nlohmann::json Connection::package(const payload &payload)
  /**
   * @brief: packages the payload into a readable input for the web request
   */
  {
    nlohmann::json data =
    {
      {"op", payload.op},
    };
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
}
