/*
 *
 */
#include "client.hpp"

/*
 * function declarations
 */
void sendMsg(int client_fd);
void receiveMsg(int client_fd);

/*
 * global variable, keeps track of client exiting
 */
bool exitClient = false;

int main(){
	/*
	 * initialize client object
	 */
	Client c;

	/*
	 *  ask user for ip address, 127.0.0.1
	 */
	string ipAddress;
	do{
		cout << "Enter hostname: ";
		cin >> ipAddress;
		if(ipAddress != "localhost"){
			cout << "Can't find the hostname, try again" << endl;
		}
	}while(ipAddress != "localhost");
	ipAddress = "127.0.0.1";

	// Create a socket for the client
	if((c.client_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
		perror("Could not make socket !!!");
		exit(EXIT_FAILURE);
	}
	else{
		cout << "Socket created" << endl;
	}

	/*
	 * initialize server address to connect
	 */
	memset(&c.address, '\0', sizeof c.address);
	c.address.sin_family = AF_INET;
	c.address.sin_port = htons(PORT);
	c.address.sin_addr.s_addr = inet_addr(ipAddress.c_str());

	// Connect to the server
	if(connect(c.client_fd, (struct sockaddr*)&c.address, sizeof(c.address)) < 0){
		perror("Failed to connect");
		close(c.client_fd);
		exit(EXIT_FAILURE);
	}
	else{
		cout << "localhost resolved to " << ipAddress << endl << "Connected" << endl << endl;
	}

	/* Initialize sending and receiving buffers
	 *
	 */
	memset(c.sendingBuff, '\0', sizeof c.sendingBuff);
	memset(c.receivingBuff, '\0', sizeof c.receivingBuff);

	/*
	 *  use of read method, reads from server, displays the message
	 */
	read(c.client_fd, c.receivingBuff, MAX);
	cout << c.receivingBuff << endl;

	/* 
	 * Get user input, send to server
	 */
	memset(c.sendingBuff,0,MAX);
	cin.getline(c.sendingBuff, MAX);
	write(c.client_fd, c.sendingBuff, MAX);

	/*
	 *  Start read and recieve threads
	 */
	thread t1(receiveMsg, c.client_fd);
	thread t2(sendMsg, c.client_fd);

	/*
	 *  Move the threads to client object
	 */
	c.threadReceiving = move(t1);
	c.threadSending = move(t2);

	/*
	 *  Wait for the receiving thread to finish
	 */
	c.threadReceiving.join();
	cout << "Joined Recieving" << endl;

	/*
	 *  use of exit method, exit the program with status code success
	 */
	exit(EXIT_SUCCESS);
	cout << "Joined Sending" << endl;

	return 0;
}

void receiveMsg(int client_fd) {
	Client c;
	while(true){
		// Clear the receiving buffer and read from the server
		memset(c.receivingBuff, 0, MAX);
		int bytesReceived = read(client_fd, c.receivingBuff, MAX);
		/*
		 *  If no bytes were received, keep listening for messages
		 */

		if(bytesReceived <= 0){
			continue;
		}
		// Display message
		cout << c.receivingBuff << endl;
		/*
		 *  If the message is "exit", set the exit flag and close the socket
		 */

		if(strcmp(c.receivingBuff, "exit") == 0){
			exitClient = true;
			close(client_fd);
			return;
		}

	}
}

void sendMsg(int client_fd) {
	Client c;
	while(true){

		if(exitClient){
			cout << "attempting exit" << endl;

			return;
		}
		memset(c.sendingBuff, 0, MAX);
		cin.getline(c.sendingBuff, MAX);
		write(client_fd, c.sendingBuff, MAX);
	}
}
