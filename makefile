CXX = clang++
CXX_FLAGS = -O2 -g -std=c++17 -Wc++17-extensions
LIBS = -lpthread \
       -lcrypto \
       -lssl \
       -lboost_system \
       -lcpprest

CPP_FILES = main.cpp\
	    bot.cpp\
	    connection.cpp\
	    utils.cpp

build:
	$(CXX) $(CXX_FLAGS) $(CPP_FILES) -o PearlBot $(LIBS)

clean:
	rm *.o
