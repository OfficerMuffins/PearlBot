#include "discord.hpp"
#include <string>
#include <iostream>

namespace discord {
  // TODO does this do what I think it does?
  Bot::Bot(std::string token, char ref) : token{U(token)}, ref{ref} {
    connection.token = this->token;
  }

  Bot::Bot() { Bot(nullptr, '\0'); }

  int Bot::run()
  {
    try {
      login();
    } catch (int e) {
      std::cout << "Failed to connect. Error code with HTTP code: " << e << std::endl;
    } catch(const boost::system::system_error& ex) {
      std::cout << "Boost exception: " << ex.code() << " " << ex.code().message() << std::endl;
    } catch (web::websockets::client::websocket_exception& e) {
      std::cout << "WS Exception: " << e.error_code() << ":" << e.what() << std::endl;
    } catch(const std::exception& e) {
      std::cout << e.what() << std::endl;
    }
    connection.end();
    return 0;
  }

  void Bot::login()
  //retrieve a response from the websocket
  {
    connection.handle_gateway();
    connection.handshake();
  }
}
