#include "Bot.hpp"
#include <sstream>

void command_makegang(const Bot &bot, std::vector users) {
  std::ostringstream out;
  for(int i = 0; i < users.size(); i++) {
    out << users[i] << " ";
  }
  bot.send_message(out);
}
