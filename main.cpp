#include "commands.hpp"
#include "bot.hpp"

#include <iostream>
#include <fstream>
#include <csignal>

using namespace std;

unique_ptr<Bot> pearlbot;

struct settings {
  vector<discord::member> gang;
};

settings bot_setting;

void signalHandler(int sig);

int main() {
  // grab the token in token.txt
  string token;
  ifstream txt_file{"token.txt"};
  getline(txt_file, token);

  pearlbot = unique_ptr<Bot>(new Bot(token, '$'));

  signal(SIGSEGV, signalHandler);
  signal(SIGTERM, signalHandler);
  signal(SIGABRT, signalHandler);

  // upload settings
  try {
    ifstream settings("settings.json");
    stringstream settings_buf;
    settings_buf << settings.rdbuf();
    auto dump = nlohmann::json::parse(settings_buf);

    pearlbot->whitelist.push_back({dump["owner"].get<uint64_t>()});
    for(auto it = dump["gang"].begin(); it != dump["gang"].end(); ++it) {
      bot_setting.gang.push_back({(*it).get<uint64_t>()});
    }

    for(auto it = dump["whitelist"].begin(); it != dump["whitelist"].end(); ++it) {
      bot_setting.gang.push_back({(*it).get<uint64_t>()});
    }

    for(auto it = dump["blacklist"].begin(); it != dump["blacklist"].end(); ++it) {
      bot_setting.gang.push_back({(*it).get<uint64_t>()});
    }
  } catch(const std::exception &e) {
    std::cout << e.what() << std::endl;
  }
  return pearlbot->run();
}

void signalHandler(int sig) {
  pearlbot.reset();
  exit(sig);
}
