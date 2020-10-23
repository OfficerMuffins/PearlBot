#pragma once
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

#define LOG()

class log : public std::runtime_error {
  private:
    std::string msg;
  public:
    log(const std::string &arg, const char* file, int line) :
      std::runtime_error(arg) {
        std::ostringstream out;
        out << file << ";" << line << ": " << arg;
        msg = out.str();
    }

    ~log() throw() {}

    const char* what() const throw() {
      return msg.c_str();
    }
};
