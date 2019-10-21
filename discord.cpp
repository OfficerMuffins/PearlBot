#include "discord.hpp"

#include <cpprest/http_client.h>
#include <cpprest/ws_client.h>
#include <nlohmann/json.hpp>
#include <iostream>

pplx::task<std::string> get_wss(const std::string &token)
/**
 * @brief: grab wss from discord response
 *
 */
{
  // building the uri
  web::uri_builder uri(U(""));
  uri.append_query(U("v"), U("6"));
  uri.append_query(U("encoding"), U("json"));
  // required headers the first request
  web::http::http_request request(web::http::methods::GET);
  // User agent fields are required for custom implemented APIs
  request.headers().add(U("Authorization"), utility::conversions::to_string_t(token));
  request.headers().add(U("Content-Type"), U("application/json"));
  request.headers().add(U("User-Agent"), U("https://github.com/OfficerMuffins/Pearlbot"));
  // uri and endpoint
  std::cout << "Preparing to send empty request" << std::endl;
  // GET is the default method, but let's be explicit
  // params: base_uri, config
  web::http::client::http_client_config config;
  config.set_validate_certificates(false);
  web::http::client::http_client client{ {U("https://discordapp.com/api/gateway")}, config };
  std::cout << client.base_uri().to_string() << std::endl;
  return client.request(request).then([](web::http::http_response response) {
      std::string wss_url;
      int code = response.status_code();
      switch(code) {
        case(200):
          return wss_url = response.extract_json().get().at("url");
        case(401):
          throw 401;
        case(402):
          throw 402;
        default:
          throw 0;
      }
    });
}

inline std::string endpoint(std::string base_uri, std::string route) { return base_uri + route; }

std::unordered_map<std::string,std::string> get_default_headers()
{
  return {
    {"Authorize", "Bot"},
    {"User-Agent", "https://github.com/Yuhanun/DPP"}
  };
};
