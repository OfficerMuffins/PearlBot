#pragma once
#include <string>

class Bot {
  private:
    std::string token;
    char ref;

  public:
    Bot(std::string, char);
    Bot();
    int run();
    void login(const std::string &);
};
