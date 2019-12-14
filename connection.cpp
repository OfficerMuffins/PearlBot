#include "discord.hpp"
#include "utils.hpp"
#include <cpprest/http_client.h>
#include <cpprest/ws_client.h>
#include <nlohmann/json.hpp>
#include <iostream>
#include <string>
#include <functional>

namespace discord {
  // using default settings
  Connection::Connection() : Connection(false) {;}

  Connection::Connection(bool compress, state status, encoding enc) : status{status}, encoding_type{enc}, compress{compress} {}

  void Connection::init() {
    using namespace std::placeholders;
    nlohmann::json dump;
    std::string uri;

    init_handles(); // assign the generators
    client.set_message_handler(std::bind(Connection::event_handler, _1, this));
    // should contain wss uri from the http gateway, only expected field is the uri field
    dump = get_wss().get();
    // FIXME hardcoded for now
    // change to add capabilities for other encodings and versions
    uri = dump.value("url", "\0") + "/?v=6&encoding=json";
    std::cout << "connecting" << std::endl;
    // connect to the gateway, expect a HELLO packet right after
    client.connect(uri).then([]() {});
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

  /**
   * @brief: sends the payload via websocket
   */
  void Connection::send_payload(const nlohmann::json &payload) {
    websocket_outgoing_message out_msg;
    out_msg.set_utf8_message(payload.dump(4));
    std::cout << "Sending " << payload.dump(4) << std::endl;
    client.send(out_msg);
    return;
  }

  // handles the event gateway
  void Connection::event_handler(const websocket_incoming_message &msg, Connection *instance) {
    std::string utf8_msg = msg.extract_string().get();
    auto parsed_json = utf8_msg.empty() ? throw "oh no" : nlohmann::json::parse(utf8_msg);
    std::cout << "message: " << parsed_json.dump(4) << std::endl;
    payload payload_msg = unpack(parsed_json);
    (instance->*(instance->handlers[payload_msg.op]))(payload_msg);
  }

  /**
   * @brief: sends a heartbeat payload at heartbeat intervals
   *
   * Discord API requires that a heartbeat payload be sent at heartbeat_interval intervals.
   * The handlers does this and sleeps for the heartbeat interval amount of time.
   *
   * @bug: how to make sure that the heartbeat thread and the other threads share clients properly?
   */
  void Connection::heartbeat() {
    // FIXME not sure what to get for sequence data
    auto f = [](Connection *instance, int interval)  {
      while(instance->status == ACTIVE) {
        auto x = std::chrono::steady_clock::now() + std::chrono::milliseconds(interval);
        send_payload(
            {
              { "op", HEARTBEAT },
              { "d", instance->last_sequence_data },
            });
        std::this_thread::sleep_until(x);
      }
    };
    heartbeat_thread(f, this, heartbeat_interval).detach();
  }

  /**
   * @brief: ends the connection with the websocket
   *
   * Declares the state dead which forces heartbeating to stop and join the main thread.
   * Once all threads have been joined, the client closes.
   */
  void Connection::end() {
    status = state::DEAD;
    /*
    for(std::vector<std::thread>::iterator it = threads.begin();
        it != threads.end();
        it++)
      it->join();*/
    //heartbeat_thread.join();
    client.close();
  }
}
