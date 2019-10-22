#include "bot.hpp"
#include "discord.hpp"

#include <iostream>

// TODO does this do what I think it does?
Bot::Bot(std::string token, char ref) : token{U(token)}, ref{ref} {}

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
  nlohmann::json gateway_dump;
  try {
    // should contain wss url, sharding information, and rate limiting
    gateway_dump = get_wss(token).get();
  } catch (int e) {
    std::cout << "Failed to connect with HTTP code: " << e << std::endl;
    exit(1);
  } catch(const boost::system::system_error& ex) {
    std::cout << ex.code() << " " << ex.code().message() << std::endl;
    exit(1);
  } catch(const std::exception& e) {
    std::cout << "login exception: " << e.what() << std::endl;
    exit(1);
  }
  // FIXME json might return different kinds of strings
  std::unordered_map<std::string, std::string> d = {
    {U("token"), this->token},
    {U("properties"), "{$os : linux, $browser: firefox, $device: laptop}"}
  }
  // discord requires us to send, information regarding bot token, platform,
  //    heartbeat rate, etc.
  send_payload(payload{discord::opcodes IDENTITY, d});
}
