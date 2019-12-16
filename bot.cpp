#include "bot.hpp"
#include "gateway.hpp"

#include <string>
#include <iostream>

Bot::Bot(std::string token, char ref)
  : token{token}, status{DISCONNECTED}, up_to_date{true}, ref{ref} {}

state* Bot::get_state_ptr() {
  return &status;
}

bool* Bot::get_up_to_date_ptr() {
  return &up_to_date;
}

int Bot::run() {
  try {
    backend::gateway wss_gateway(*this, false, backend::JSON);
  } catch(const std::exception& e) {
    std::cout << e.what() << std::endl;
  }
  return 0;
}
