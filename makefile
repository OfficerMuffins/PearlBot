CXX = clang++
CXX_FLAGS = -O2 -std=c++17 -Og -Wc++17-extensions
LIBS = -lpthread \
       -lcrypto \
       -lssl \
       -lboost_system \
       -lcpprest

CPP_FILES = main.cpp\
	    bot.cpp\
	    discord.cpp

build:
	$(CXX) $(CPP_FILES) -o PearlBot $(LIBS)

clean:
	rm *.o
