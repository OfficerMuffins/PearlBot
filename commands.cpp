#include "commands.hpp"
//#include <boost/python.hpp>
#include <sstream>

#define CREATE_INSTANT_INVITE	0x00000001
#define KICK_MEMBERS 	        0x00000002
#define BAN_MEMBERS 	        0x00000004
#define ADMINISTRATOR 	        0x00000008
#define MANAGE_CHANNELS 	0x00000010
#define MANAGE_GUILD 	        0x00000020
#define ADD_REACTIONS	        0x00000040
#define VIEW_AUDIT_LOG	        0x00000080
#define VIEW_CHANNEL	        0x00000400
#define SEND_MESSAGES	        0x00000800
#define SEND_TTS_MESSAGES	0x00001000
#define MANAGE_MESSAGES 	0x00002000
#define EMBED_LINKS	        0x00004000
#define ATTACH_FILES	        0x00008000
#define READ_MESSAGE_HISTORY	0x00010000
#define MENTION_EVERYONE	0x00020000
#define USE_EXTERNAL_EMOJIS	0x00040000
#define CONNECT	                0x00100000
#define SPEAK	                0x00200000
#define MUTE_MEMBERS	        0x00400000
#define DEAFEN_MEMBERS	        0x00800000
#define MOVE_MEMBERS	        0x01000000
#define USE_VAD	                0x02000000
#define PRIORITY_SPEAKER	0x00000100
#define STREAM	                0x00000200
#define CHANGE_NICKNAME	        0x04000000
#define MANAGE_NICKNAMES	0x08000000
#define MANAGE_ROLES 	        0x10000000
#define MANAGE_WEBHOOKS 	0x20000000
#define MANAGE_EMOJIS 	        0x40000000

using namespace std;

struct settings {
  vector<discord::user> gang;
};

extern settings bot_setting;
extern std::unique_ptr<Bot> pearlbot;

void command_makegang(std::vector<discord::user> users) {
  if((pearlbot->bot_role.permissions & SEND_MESSAGES) == 0) { // no permission to send message
    throw std::runtime_error("Permission Denied");
  }

  std::ostringstream out;
  out << "Gay gang: ";
  for(const discord::user& usr : users) {
    bot_setting.gang.push_back(usr);
    out << usr.username << " ";
  }
  pearlbot->create_message(out.str());
}

void command_pinggang() {
  if((pearlbot->bot_role.permissions & SEND_MESSAGES) == 0) { // no permission to send message
    throw std::runtime_error("Permission Denied");
  }

  std::ostringstream out;
  for(const discord::user& usr : bot_setting.gang) {
    out << usr << " ";
  }
  pearlbot->create_message(out.str());
}

/*
void command_lookup_league(std::string username, std::string region) {
}*/

void command_hello() {
  pearlbot->create_message("Hello");
}

void command_permission_denied() {
  pearlbot->create_message("Permission Denied");
}
