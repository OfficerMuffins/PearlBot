CXX = clang++
CXX_FLAGS = -O0 -g -std=c++17 -Wc++17-extensions -Wall
LIBS = -lpthread \
       -lcrypto \
       -lssl \
       -L/usr/local/lib/libboost_system.so.1.69.0 \
       -lcpprest

INCLUDE_DIR = include
DISCORD_SRC = src/discord
CPP_FILES = $(DISCORD_SRC)/main.cpp\
	    $(DISCORD_SRC)/bot.cpp\
	    $(DISCORD_SRC)/connection.cpp\
	    $(DISCORD_SRC)/utils.cpp\
	    $(DISCORD_SRC)/gateway.cpp\
	    $(DISCORD_SRC)/events.cpp\
	    $(DISCORD_SRC)/endpoints.cpp\
	    $(DISCORD_SRC)/discord.cpp\
	    $(DISCORD_SRC)/commands.cpp

build:
	$(CXX) $(CXX_FLAGS) -I $(INCLUDE_DIR) $(CPP_FILES) -o PearlBot $(LIBS)

clean:
	rm *.o
