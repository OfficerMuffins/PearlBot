#pragma once
#include "bot.hpp"

/*
struct Function {
  virtual ~Fuction() {}
};

template <typename T>
struct command : Function {
  std::function<T> f;
  command(std::function<T> f) : f(f) {}
};*/

void command_makegang(std::vector<discord::user>);
void command_pinggang();
