#include "bot.hpp"
#include "gateway.hpp"

#include <string>
#include <iostream>

std::string gaygang[4];

/**
 * @brief: bot constructor
 *
 * Constructor for bot.
 *
 * @param[in]: token, the bot's token
 * @param[in]: ref, a reference for the discord users to use to ping the bot
 */
Bot::Bot(std::string token, char ref)
  : status{DISCONNECTED}, up_to_date{true}, token{token}, ref{ref} {}

state* Bot::get_state_ptr() { return &status; }

bool* Bot::get_up_to_date_ptr() { return &up_to_date; }

int Bot::run() {
  try {
    backend::gateway wss_gateway(*this, false, backend::JSON);
    std::thread wss = std::thread([&wss_gateway](){wss_gateway.run();}); // start the gateway handler
    wss.join();
  } catch(const std::exception& e) {
    std::cout << e.what() << std::endl;
  }
  return 0;
}

void Bot::create_message() {
}
