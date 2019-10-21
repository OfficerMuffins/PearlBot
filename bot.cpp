#include "bot.hpp"
#include "discord.hpp"

#include <iostream>

// TODO does this do what I think it does?
Bot::Bot(std::string token, char ref) : token{token}, ref{ref} {}

int
Bot::run()
{
  std::cout << "printing" << std::endl;
  login(this->token);
  return 0;
}

void
Bot::login(const std::string &token)
//retrieve a response from the websocket
{
  try {
    auto response = get_wss(token).get();
  } catch (401) {
    exit(1);
  } catch(402) {
    exit(1);
  } catch(const boost::system::system_error& ex) {
    std::cout << ex.code() << " " << ex.code().message() << std::endl;
    exit(1);
  } catch(const std::exception& e) {
    std::cout << "login exception: " << e.what() << std::endl;
    exit(1);
  }
}
