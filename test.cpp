#include <cpprest/http_client.h>
#include <nlohmann/json.hpp>
#include <iostream>

pplx::task<web::http::http_response> get_wss(const std::string &token)
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
  request.set_request_uri(utility::conversions::to_string_t("gateway/bot"));
  request.headers().add(U("Authorization"), utility::conversions::to_string_t(token));
  request.headers().add(U("Content-Type"), U("application/json"));
  // uri and endpoint
  std::cout << "Preparing to send empty request" << std::endl;
  // GET is the default method, but let's be explicit
  // params: base_uri, config
  web::http::client::http_client_config config;
  config.set_validate_certificates(false);
  web::http::client::http_client client{ {U("https://discordapp.com/api/v6")}, config };
  std::cout << client.base_uri().to_string() << std::endl;
  return client.request(request).then([](web::http::http_response response) {
      try {
          std::cout << "Received code "<< response.status_code() << std::endl;
        } catch (const web::http::http_exception &e){
          std::cout << "get_wss exception:" << e.what() << std::endl;
        }

        return response;
      });
}

int main() {
  auto task = get_wss("hereersdf").get();
}
