#include "discord.hpp"
#include "bot.hpp"

int
main()
{
  // grab the token in token.txt
  std::string token;
  std::ifstream txt_file{"token.txt"};
  std::getline(txt_file, token);
  // make a connection
  discord::Bot pearlbot{token, '#'};
  return pearlbot.run();
}
