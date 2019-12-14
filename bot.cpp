#include "discord.hpp"
#include <string>
#include <iostream>

namespace discord {
  Bot::Bot(std::string token, char ref) : token{U(token)}, ref{ref}, connection(false, ACTIVE, JSON) {
    connection.token = this->token;
    connection.init();
  }

  int Bot::run() {
    try {
    } catch (int e) {
      std::cout << "Failed to connect. Error code with HTTP code: " << e << std::endl;
    } catch(const boost::system::system_error& ex) {
      std::cout << "Boost exception: " << ex.code() << " " << ex.code().message() << std::endl;
    } catch (web::websockets::client::websocket_exception& e) {
      std::cout << "WS Exception: " << e.error_code() << ":" << e.what() << std::endl;
    } catch(const std::exception& e) {
      std::cout << e.what() << std::endl;
    }
    while(1);
    return 0;
  }

  //retrieve a response from the websocket
  void Bot::login() {
  }
}
