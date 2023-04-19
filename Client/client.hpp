/* file name: client.hpp
 * course: COP4635
 * Project: 3, part1
 * Author: Jeremy Caole, Kevin Teague
 * Description: header file for client class
 */

#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <cstring>
#include <iostream>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <algorithm>
#include <iostream>
#include <sstream>
#include <vector>
#include <fstream>
#include <thread>
#include <mutex>

#define PORT 60012
#define MAX 5000

using namespace std;

class Client{
	public:
		int client_fd;
		char receivingBuff[MAX]{};
		char sendingBuff[MAX]{};
		int new_socket{};
		struct sockaddr_in address{};
		int addrLen{};
		bool exitClient = false;
		std::thread threadReceiving;
		std::thread threadSending;

};

#endif //CLIENT_HPP
