#include "bot.hpp"

#include <iostream>
#include <fstream>

using namespace std;
int main() {
  // grab the token in token.txt
  string token;
  ifstream txt_file{"token.txt"};
  getline(txt_file, token);

  string owner_id;
  getline(txt_file, owner_id);
  // make a connection
  Bot pearlbot{stoul(owner_id), token, '#'};
  return pearlbot.run();
}
