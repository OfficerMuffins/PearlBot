#include "discord.hpp"
#include "bot.hpp"

using namespace discord;

namespace discord {
  Bot pearlbot;
}

int
main()
{
  // grab the token in token.txt
  std::string token;
  std::ifstream txt_file{"token.txt"};
  std::getline(txt_file, token);
  // make a connection
  pearlbot = {token, '#'};
  return pearlbot.run();
}
