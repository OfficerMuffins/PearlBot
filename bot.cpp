#include "connection.hpp"
#include "endpoints.hpp"
#include "gateway.hpp"
#include "bot.hpp"

#include <string>
#include <iostream>

/**
 * @brief: bot constructor
 *
 * Constructor for bot.
 *
 * @param[in]: token, the bot's token
 * @param[in]: ref, a reference for the discord users to use to ping the bot
 */
Bot::Bot(uint64_t owner_id, std::string token, char ref)
  : status{DISCONNECTED}, up_to_date{true}, wss_gateway(this, false, backend::JSON), c(this), token{token}, ref{ref} {
    whitelist.push_back({owner_id});
  }

int Bot::run() {
  try {
    c.identify(); // establish the connection
    std::thread wss = std::thread([this](){this->wss_gateway.run();}); // start the gateway handler
    while(status == ACTIVE) {
      if(!command_q.empty()) {
        std::string cmd = command_q.front();
        command_q.pop(); // service the commands
        if(cmd == "gaygang") {
          // ping everyone
          //create_message();
        }
      }
    }
    wss.join();
  } catch(const std::exception& e) {
    std::cout << __FILE__  << __LINE__ << e.what() << std::endl;
  }
  return 0;
}
