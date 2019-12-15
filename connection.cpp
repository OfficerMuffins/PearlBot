#include "discord.hpp"
#include <cpprest/http_client.h>
#include <cpprest/ws_client.h>
#include <nlohmann/json.hpp>
#include <iostream>
#include <string>
#include <functional>
#include <future>

namespace discord {
  // using default settings
  Connection::Connection() : Connection(false) {;}

  Connection::Connection(bool compress, state status, encoding enc) :
    status{status}, encoding_type{enc}, compress{compress} {}

  /**
   * @brief: run the connection
   *
   * A call to this function is blocking.
   */
  void Connection::run() {
    using namespace std::placeholders;
    nlohmann::json dump;
    std::string uri;

    // message handler can't be member function
    client.set_message_handler([this](const websocket_incoming_message &msg){ this->handle_callback(msg); });
    // should contain wss uri from the http gateway, only expected field is the uri field
    dump = get_wss().get();
    // FIXME hardcoded for now
    // change to add capabilities for other encodings and versions
    uri = dump.value("url", "\0") + "/?v=6&encoding=json";
    std::cout << "connecting" << std::endl;
    // connect to the gateway, expect a HELLO packet right after
    status = NEW;
    up_to_date = true;
    client.connect(uri).then([]() {});

    std::promise<void> p;
    std::future<void> f = p.get_future();
    while(status == NEW); // give sometime to receive the HELLO event
    while(true) { // continually try to maintain a connection
      try {
        //resource_manager = std::thread([this] { this->manage_resources(); });
        heartbeat_thread = std::thread([&]{ this->heartbeat(p); });
        //event_thread = std::thread([this] { this->manage_events(); });
        heartbeat_thread.join(); // if returns, we have disconnected
        //resource_manager.join();
        //event_thread.join(); // disconnects when heartbeating fails
        f.get(); // throw error from the heartbeat thread
      } catch(const std::runtime_error &err) { // disconnected, send a resume payload
        std::cout << "disconnected!" << std::endl;
        up_to_date = false;
        reconnect();
      } catch(const std::exception &e) { // unexpected exception, time to leave
        std::cout << e.what() << std::endl;
        throw std::runtime_error("Unexpected exception during heartbeat");
      }
    }
  }

  void Connection::reconnect() {
    client.close(wss_close_status::abnormal_close);
    std::cout << uri << std::endl;
    client.connect(uri).then([]() {});
  }

  /**
   * @brief: sends the payload via websocket
   *
   * @param[in]: payload reference that will be sent
   */
  void Connection::send_payload(const nlohmann::json &payload) {
    websocket_outgoing_message out_msg;
    out_msg.set_utf8_message(payload.dump(4));
    std::cout << "Sending " << payload.dump(4) << std::endl;
    client_lock.lock();
    client.send(out_msg);
    client_lock.unlock();
    return;
  }

  /**
   * @brief: client callback handler
   *
   * Handles incoming payloads by using the provided opcode as key.
   *
   * @param[in]: incoming message. Use to extract json
   */
  void Connection::handle_callback(const websocket_incoming_message &msg) {
    //assert(status != DISCONNECTED); // we shouldn't be receiving packets when disconnected
    std::string utf8_msg = msg.extract_string().get();
    auto parsed_json = utf8_msg.empty() ? throw "oh no" : nlohmann::json::parse(utf8_msg);
    std::cout << "message: " << parsed_json.dump(4) << std::endl;
    payload payload_msg = unpack(parsed_json);
    (this->*(this->handlers[payload_msg.op]))(payload_msg);
  }

  /**
   * @brief: sends a heartbeat payload at heartbeat intervals
   *
   * Discord API requires that a heartbeat payload be sent at heartbeat_interval intervals.
   * The number of heartbeat is kept in a variable heartbeat_ticks which is incremented everytime a
   * heartbeat is sent and decrement when a HEARTBEAT_ACK is received, therefore it should always be
   * 0 at the start of sending the heartbeat;
   *
   * @bug: how to make sure that the heartbeat thread and the other threads share clients properly?
   * @bug: concurrency issues with heartbeat?
   */
  void Connection::heartbeat(std::promise<void> &p) {
    // FIXME not sure what to get for sequence data
    try {
      while(status == ACTIVE) {
        // serialize the main thread which can be sending heartbeats in response
        heartbeat_lock.lock();
        if(heartbeat_ticks != 0) {
          std::cout << "this is why we disconnected" << std::endl;
          status = DISCONNECTED;
          heartbeat_lock.unlock();
          throw std::runtime_error("did not receive heartbeat ack");
        }
        std::cout << "sending heartbeat" << heartbeat_interval << std::endl;
        heartbeat_ticks++; // increment the ticks to show that sent out a heartbeat
        // heartbeat_interval is maximum amount of time and there's no punishment for
        // heartbeating, so send it a little earlier
        auto x = std::chrono::steady_clock::now() +
          std::chrono::milliseconds(heartbeat_interval);
        send_payload(
            {
              { "op", HEARTBEAT },
              { "d", last_sequence_data },
            });
        heartbeat_lock.unlock();
        std::cout << "sleeping" << std::endl;
        std::this_thread::sleep_until(x);
      }
      return;
    } catch(const std::exception &err) {
      p.set_exception(std::current_exception());
      return;
    }
  }

  /**
   * @brief: manage sending of events
   *
   * While the connection is active, ths function manages the sending of events while
   * implementing rate limiting. This thread uses a semaphore that is decremented everytime
   * an event is sent to server. The semaphore will block at 0 which means that the connection
   * is reaching its event rate limit. The function will block until the semaphore is reset by
   * the resource manager thread.
   */
  void Connection::manage_events() {
    while(status == ACTIVE) {
      event_q_lock.lock();
      if(event_q.empty()) {
        event_q_lock.unlock();
        continue;
      }
      payload p = event_q.front();
      send_payload(package(p));
      event_q.pop();
      event_q_lock.unlock();
      rate_sem.wait();
    }
    return;
  }

  /**
   * @brief: ends the connection with the websocket
   *
   * Declares the state dead which forces heartbeating to stop and join the main thread.
   * Once all threads have been joined, the client closes.
   */
  /*
  void Connection::close() {
    status = state::DEAD;
    heartbeat_thread.join();
    client.close();
  }*/

  /**
   * @brief: manages the resources of the connection
   */
  void Connection::manage_resources() {
    while(status == ACTIVE) {
      // reset the semaphore every minute
      auto x = std::chrono::steady_clock::now() + std::chrono::seconds(60);
      std::this_thread::sleep_until(x);
      rate_sem.reset();
    }
    return;
  }
}
