#include "utils.hpp"

void extendJson(nlohmann::json &j1, const nlohmann::json &j2) {
  for (const auto &j : nlohmann::json::iterator_wrapper(j2)) {
    j1[j.key()] = j.value();
  }
}
