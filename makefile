# filename:
# course:
# Project:
# Author:
# Description:

CXX = g++
CXXFLAGS = -g -std=c++11 -Wall -pthread -Wall

SRCS = $(wildcard *.hpp)
OBJECTS = $(SRCS: .hpp=.o)

all: server client

server: $(OBJECTS) Server/main.cpp Server/user.hpp Server/user.cpp Server/server.cpp Server/server.hpp
	$(CXX) $(CXXFLAGS) -o $@ $^

client: $(OBJECTS) Client/main.cpp Client/client.hpp
	$(CXX) $(CXXFLAGS) -o $@ $^

clean:
	$(RM) *.o  test/*.o core server client
