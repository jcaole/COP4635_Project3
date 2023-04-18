#ifndef SERVER_HPP
#define SERVER_HPP

//standard C library headers:
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <cstring>

//Standard C++ library headers:
#include <iostream>
#include <algorithm>
#include <vector>
#include <fstream>
#include <thread>

//POSIX socket headers:
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <string.h>
#include <thread>
#include <mutex>
#include <fstream>
#include <sstream>
#include <map>
#include "user.hpp"

#define MAX 5000
#define PORT 60012

using namespace std;


class Server{
	public:       
		mutex mtx;
		int server_fd;
		int new_socket;
		int id;
		char sendingBuff[MAX];
		char receivingBuff[MAX];
		struct sockaddr_in address;
		int addrLen;

		vector<thread> threads;
		vector<User> users;
		int usersActive;
		Server();

		bool loggedIn = false;

		bool Login(int new_socket, int id);

		void Register(int new_socket);

		bool mainMenu(int new_socket, int id);

		bool checkLogin(string username, string password);
		void optionsWhenLoggedIn(int new_socket);        

		void subscribe(int new_socket, int id);

		void unsubscribe(int new_socket, int id);

		void seeLocations(int new_socket, int id); 

		void changePassword(int new_socket, int id);        
		void exitProgram(int new_socket, int id);

		void run(int socket, int id);
};
#endif //SERVER_HPP

