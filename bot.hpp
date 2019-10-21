#pragma once
#include <string>

#ifndef __BOT_HPP__
#define __BOT_HPP__
class Bot {
  private:
    std::string token;
    char ref;

  public:
    Bot(std::string, char);
    Bot() : ref{'\0'}, token{std::string{"."}} {};
    int run();
    void login(const std::string &);
};
#endif
