CXX = clang++
CXX_FLAGS = -Og
LIBS = -lpthread \
       -lcrypto \
       -lssl \
       -lboost_system \
       -lcpprest

CPP_FILES = main.cpp\
	    bot.cpp\
	    discord.cpp

build:
	$(CXX) $(CPP_FILES) $(LIBS)

clean:
	rm *.o
