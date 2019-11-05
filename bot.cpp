#include "discord.hpp"
#include "utils.hpp"
#include <iostream>

namespace discord {
  wss_client client;
  int sharding;
  snowflake_t snowflake;

  // TODO does this do what I think it does?
  Bot::Bot(std::string token, char ref) : token{U(token)}, ref{ref} {;}

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
    std::string wss_url;
    try {
      // should contain wss url, only expected field is the url field
      gateway_dump = get_wss(token).get();
      wss_url = gateway_dump.at("url");
      std::cout << wss_url << std::endl;
    } catch (int e) {
      std::cout << "Failed to connect. Error code with HTTP code: " << e << std::endl;
      exit(1);
    } catch(const boost::system::system_error& ex) {
      std::cout << "Boost exception: " << ex.code() << " " << ex.code().message() << std::endl;
      exit(1);
    } catch(const std::exception& e) {
      std::cout << "login exception: " << e.what() << std::endl;
      exit(1);
    }
    // FIXME json might return different kinds of strings
    /*
    std::unordered_map<std::string, std::string> d = {
      {U("token"), this->token},
      {U("properties"), "{$os : linux, $browser: firefox, $device: laptop}"}
    }
    // discord requires us to send, information regarding bot token, platform,
    //    heartbeat rate, etc.
    send_payload(payload{discord::opcodes IDENTITY, d});*/
  }
}

