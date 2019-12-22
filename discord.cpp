#include "discord.hpp"

namespace discord {
  payload::payload(opcodes op, nlohmann::json d, int s, std::string t) :
    op{op}, d{d}, s{s}, t{t} {;}

  user::user(uint64_t id) : id{id} {;}
  user::user() : id{0} {;}

  // write the user object as a ping
  std::ostream& operator<<(std::ostream& os, const user& obj) {
    os << "<@" << obj.id << ">";
    return os;
  }
}
