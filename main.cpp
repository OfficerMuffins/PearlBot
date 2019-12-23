#include "commands.hpp"
#include "bot.hpp"

#include <iostream>
#include <fstream>

using namespace std;

unique_ptr<Bot> pearlbot;

struct settings {
  vector<discord::user> gang;
};

settings bot_setting;

int main() {
  // grab the token in token.txt
  string token;
  ifstream txt_file{"token.txt"};
  getline(txt_file, token);

  pearlbot = unique_ptr<Bot>(new Bot(token, '$'));

  // upload settings
  try {
    ifstream settings("settings.json");
    stringstream settings_buf;
    settings_buf << settings.rdbuf();
    auto dump = nlohmann::json::parse(settings_buf);

    pearlbot->whitelist.push_back({dump["owner"].get<uint64_t>()});
    size_t idx = 0;
    while(!dump["gang"][idx++].is_null()) {
      bot_setting.gang.push_back({!dump["gang"][idx].get<uint64_t>()});
    }

    while(!dump["whitelist"][idx++].is_null()) {
      bot_setting.gang.push_back({!dump["gang"][idx].get<uint64_t>()});
    }

    while(!dump["blacklist"][idx++].is_null()) {
      bot_setting.gang.push_back({!dump["gang"][idx].get<uint64_t>()});
    }
  } catch(const std::exception &e) {
    std::cout << "Failed to load settings" << e.what() << std::endl;
  }
  return pearlbot->run();
}
