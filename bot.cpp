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
Bot::Bot(std::string token, char ref)
  : status{DISCONNECTED}, up_to_date{true}, token{token}, ref{ref}, wss_gateway(false, backend::JSON) {
    wss_gateway.init_shared(&status, &up_to_date, token);
    c.init_shared(&status, &up_to_date, token);
  }

int Bot::run() {
  try {
    command_q = std::make_shared<std::queue<commands>>();
    wss_gateway.init_cmd_q(command_q);
    c.init_cmd_q(command_q);
    c.identify(); // establish the connection
    std::thread wss = std::thread([this](){this->wss_gateway.run();}); // start the gateway handler
    while(status == ACTIVE) {
      if(!command_q->empty()) {
        commands c = command_q->front();
        command_q->pop(); // service the commands
        if(c == GAYGANG) {
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
