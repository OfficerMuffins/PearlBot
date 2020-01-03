#include "discord.hpp"
#include "utils.hpp"
#include "gateway.hpp"
#include "bot.hpp"

#define PAYLOAD_DEBUG 1
namespace backend {
  // using default settings
  gateway::gateway(Bot *const bot, bool compress, encoding enc) : Connection(bot), workers(NUM_THREADS), rate_sem{rate_limit} {
    // set the websocket call back and close handlers
    client.set_message_handler([this](const websocket_incoming_message &msg){ this->handle_callback(msg); });
    client.set_close_handler([this](const wss_close_status close_status, const utility::string_t &reason, const std::error_code &error) {
        std::cout << "error " << error.value() << " " << static_cast<int>(close_status) << ":" << reason << std::endl;
        this->close();
        });
  }

  /**
   * @brief: run the connection
   *
   * A call to this function is blocking and this funciton should be called via a seperate thread
   */
  void gateway::run() {
    nlohmann::json dump;

    // set the new connections status
    bot->status = NEW;
    bot->up_to_date = true;

    // connect to the gateway, expect a HELLO packet right after
    client.connect(uri).then([](){});

    // future to grab exception by the heartbeat thread
    // TODO for now, only the exception from the heartbeat thread is caught
    std::promise<void> p;
    std::future<void> f = p.get_future();

    while(bot->status != ACTIVE); // wait until we receive the ready event for us to actually start heartbeating

    // TODO if disconnected, signal all other threads from other files
    while(bot->status != TERMINATE) { // continually try to maintain a connection
      try {
        // start all workers, for now, rate limiting is turned off because low activity is expected
        boost::asio::post(workers, [&]{ this->heartbeat(p); }); // sends heartbeats at hearbeat intervals
        //boost::asio::post(workers, [this] { this->manage_resources(); }); // resets the rate limit
        //boost::asio::post(workers, [this] { this->manage_events(); }); // sends events at rate limit
        workers.join();
        f.get(); // throw exception from the heartbeat thread
      } catch(const std::runtime_error &err) { // disconnected, send a resume payload
        std::cout << "disconnected!" << std::endl;
        bot->up_to_date = false;
        reconnect();
      } catch(const std::exception &e) { // unexpected exception, time to leave
        std::cout << __FILE__ << __LINE__ << ": " << e.what() << std::endl;
        throw std::runtime_error("Unexpected exception during heartbeat");
      }
    }
  }

  /**
   * @brief:
   */
  void gateway::reconnect() {
    client.connect(uri).then([]() {});
    send_payload(package({discord::RESUME}));
  }

  /**
   * @brief: sends the payload via websocket
   *
   * @param[in]: payload reference that will be sent
   */
  void gateway::send_payload(const nlohmann::json &payload) {
    websocket_outgoing_message out_msg;
    out_msg.set_utf8_message(payload.dump(4));
#if PAYLOAD_DEBUG == 1
    std::cout << "Sending to " << client.uri().to_string() << std::endl << payload.dump(4) << std::endl;
#else
#endif
    client_lock.lock();
    client.send(out_msg).then([](){});
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
  void gateway::handle_callback(const websocket_incoming_message &msg) {
    std::string utf8_msg = msg.extract_string().get();
    auto parsed_json = utf8_msg.empty() ? throw "oh no" : nlohmann::json::parse(utf8_msg);
#if PAYLOAD_DEBUG == 1
    std::cout << "message: " << parsed_json.dump(4) << std::endl;
#else
#endif
    discord::payload payload_msg = unpack(parsed_json);
    if(bot->up_to_date == true) { // if this is the most recent event, then we are ok with sending it
      try {
        (this->*(this->handlers[payload_msg.op]))(payload_msg);
      } catch(const std::exception &e) { // TODO handle this
        std::cout << __FILE__ << __LINE__ << ": " << e.what() << std::endl;
        close();
        exit(2);
      }
    } else { // we should record all events that have occured until we are resumed
      while(payload_msg.t != "RESUMED") {
      }
      bot->up_to_date = true;
    }
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
  void gateway::heartbeat(std::promise<void> &p) {
    // FIXME not sure what to get for sequence data
    try {
      while(bot->status == ACTIVE) {
        // serialize the main thread which can be sending heartbeats in response
        heartbeat_lock.lock();
        if(heartbeat_ticks != 0) {
          bot->status = DISCONNECTED;
          heartbeat_lock.unlock();
          throw std::runtime_error("did not receive heartbeat ack");
        }
        heartbeat_ticks++; // increment the ticks to show that sent out a heartbeat
        // heartbeat_interval is maximum amount of time and there's no punishment for
        // heartbeating, so send it a little earlier
        auto x = std::chrono::steady_clock::now() +
          std::chrono::milliseconds(heartbeat_interval);
        send_payload(
            {
              { "op", discord::HEARTBEAT },
              { "d", last_sequence_data },
            });
        heartbeat_lock.unlock();
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
  void gateway::manage_events() {
    try {
      while(bot->status == ACTIVE) {
        event_q_lock.lock();
        if(event_q.empty()) {
          event_q_lock.unlock();
          continue;
        }
        discord::payload p = event_q.front();
        send_payload(package(p));
        event_q.pop();
        event_q_lock.unlock();
        rate_sem.wait();
      }
      return;
    } catch(const std::exception &e) {
      std::cout << __FILE__ << __LINE__ << ": " << e.what() << std::endl;
    }
  }

  /**
   * @brief: ends the connection with the websocket
   *
   * Declares the state dead which forces heartbeating to stop and join the main thread.
   * Once all threads have been joined, the client closes.
   */
  void gateway::close() {
    bot->status = TERMINATE;
    workers.join(); // peaceful exit
    client.close().then([](){});
  }

  /**
   * @brief: manages the resources of the connection
   *
   * For now, all this thread does is reset the wait limit of the event semaphore.
   */
  void gateway::manage_resources() {
    try {
      while(bot->status == ACTIVE) {
        // reset the semaphore every minute
        auto x = std::chrono::steady_clock::now() + std::chrono::seconds(60);
        std::this_thread::sleep_until(x);
        rate_sem.reset();
      }
      return;
    } catch(const std::exception &e) {
      std::cout << __FILE__ << __LINE__ << ": " << e.what() << std::endl;
    }
  }
}
