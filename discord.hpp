#pragma once
#include "bot.hpp"

#include <nlohmann/json.hpp>
#include <cpprest/http_client.h>
#include <string>

#ifndef __DISCORD_HPP__
#define __DISCORD_HPP__
namespace discord
{
  struct payload {
    char op;
  };
}

pplx::task<web::http::http_response> get_wss(const std::string &);
inline std::string endpoint(std::string, std::string);
std::unordered_map<std::string, std::string> get_default_headers();
nlohmann::json package(const discord::payload&);
#endif
