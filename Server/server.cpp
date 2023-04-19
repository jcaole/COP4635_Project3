/*
 *
 */
#include "server.hpp"

Server::Server() {
	// initialize some variables
	id = 0;
	usersActive = 0;
	addrLen = sizeof(address);

	// create a socket file descriptor
	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
		perror("ERROR: first socket if statement");
		exit(EXIT_FAILURE);
	}

	// set the properties of the address struct
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(PORT);
	memset(address.sin_zero, '\0', sizeof address.sin_zero);

	// bind the socket to the address
	if (bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0) {
		perror("ERROR: bind if statement");
		exit(EXIT_FAILURE);
	}
	cout << "bind done" << endl;

	// start listening for connections
	if (listen(server_fd, 10) < 0) {
		perror("ERROR: listen if statement");
		exit(EXIT_FAILURE);
	}

	// accept incoming connections
	while(true) {
		printf("Waiting for incoming connections...\n");
		if((new_socket = accept(server_fd, (struct sockaddr *) &address, (socklen_t *) &addrLen)) < 0){
			perror("ERROR: while loop, accept connections if statement");
			exit(EXIT_FAILURE);
		}

		// create a new thread to handle the connection
		id++;
		threads.push_back(std::thread(&Server::run, this, new_socket, id));
	}

	// join all the threads
	for (size_t i = 0; i < threads.size(); i++) {
		if (threads[i].joinable())
			threads[i].join();
	}
}

/*
 *  run method, main method for server, runs server for the client with the given ID and new socket.
 */
void Server::run(int new_socket, int id){
	cout << "Connection accepted" << endl;
	cout << "Handler assigned" << endl;
	cout << "ID #" << id << endl;

	// Initialize a boolean variable indicating whether or not the user is still connected to the server.
	bool connected = true;

	// Call the main menu function to display options to the user and wait for their input.
	connected = mainMenu(new_socket, id);

	// While the user is logged in, continue receiving and processing their input.
	while(loggedIn){
		// Clear the buffer that will be used to store the user's input.
		memset(receivingBuff, 0, MAX);

		// Read from the socket into the buffer.
		read(new_socket, receivingBuff, (size_t)MAX);

		// call the appropriate function.
		if(strcmp(receivingBuff, "1") == 0){
			subscribe(new_socket, id);
		}
		else if(strcmp(receivingBuff, "2") == 0){
			unsubscribe(new_socket, id);
		}
		else if(strcmp(receivingBuff, "3") == 0){
			seeLocations(new_socket, id);
		}
		else if(strcmp(receivingBuff, "4") == 0){
			changePassword(new_socket, id);
		}
		else if(strcmp(receivingBuff, "5") == 0){
			// Log the user out and remove them from the list of active users.
			loggedIn = false;
			for (size_t i = 0; i < users.size(); i++)
			{
				if (users[i].getId() == id)
				{
					users.erase(users.begin() + i);
					break;
				}
			}
			usersActive--;
			// Return to the main menu.
			connected = mainMenu(new_socket, id);
		}
		else if(strcmp(receivingBuff, "0") == 0) {
			// If the user selects option 0, exit the program.
			exitProgram(new_socket, id);
			return;
		}

		// If the user enters an invalid option, inform them and display the main menu again.
		else{
			memset(sendingBuff, 0, MAX);
			string invalid = "Invalid choice, please try again";
			strcpy(sendingBuff, invalid.c_str());
			write(new_socket, sendingBuff, (int)MAX);
			optionsWhenLoggedIn(new_socket);
		}
	}

	// If the user has disconnected, return from the function.
	if(!connected){
		return;
	}
}

/*
 * login method, handle the login of a user.
 */
bool Server::Login(int new_socket, int id) {
	// Reset the receiving and sending buffers.
	memset(receivingBuff, 0, MAX);
	memset(sendingBuff, 0, MAX);

	// Get the username from the client.
	string username, password;
	string uName = " Username:\n";

	// Send a request for the username to the client.
	strcpy(sendingBuff, uName.c_str());
	write(new_socket, sendingBuff, (int)MAX);

	// Receive the username from the client.
	memset(receivingBuff, 0, MAX);
	read(new_socket, receivingBuff, (size_t)MAX);
	username = receivingBuff;

	// Send a request for the password to the client.
	memset(sendingBuff, 0, MAX);
	string pWord = " Password:\n";
	strcpy(sendingBuff, pWord.c_str());
	write(new_socket, sendingBuff, (int)MAX);

	// Receive the password from the client.
	read(new_socket, receivingBuff, (size_t)MAX);
	password = receivingBuff;
	cout << "u: " << username << " p: "  << password << endl;

	// Check if the login credentials are valid.
	if(checkLogin(username, password)){
		// Create a new user and add it to the list of users.
		User userr(username, password, new_socket, id);
		users.push_back(userr);
		usersActive++;
		cout << "Users active: " << usersActive << endl;

		// Notify the client that the login was successful.
		memset(sendingBuff, 0, MAX);
		string logged = "Successfully Logged In";
		strcpy(sendingBuff, logged.c_str());
		write(new_socket, sendingBuff, (int)MAX);
		return true;
	}
	else{
		// Notify the client that the login was unsuccessful.
		return false;
	}
}

/*
 * register method
 */
void Server::Register(int new_socket){
	// lock the mutex to prevent multiple threads from accessing shared resources at the same time
	mtx.lock();

	// clear receiving and sending buffers
	memset(receivingBuff, 0, MAX);
	memset(sendingBuff, 0, MAX);

	// prompt user for username
	string username, password;
	string uName = " Username:\n";
	strcpy(sendingBuff, uName.c_str());
	write(new_socket, sendingBuff, (int)MAX);
	memset(receivingBuff, 0, MAX);
	read(new_socket, receivingBuff, (size_t)MAX);
	username = receivingBuff;

	// prompt user for password
	memset(sendingBuff, 0, MAX);
	string pWord = " Password:\n";
	strcpy(sendingBuff, pWord.c_str());
	write(new_socket, sendingBuff, (int)MAX);
	read(new_socket, receivingBuff, (size_t)MAX);
	password = receivingBuff;
	cout << "u: " << username << " p: "  << password << endl;

	// write new user to file
	ofstream usersFile;
	usersFile.open("users.txt", fstream::app);
	if (usersFile.is_open())
	{
		usersFile << username << "\t" << password << endl;
		usersFile.close();
	}
	else{
		cout << "Unable to open file";
	}

	// send success message to client
	memset(sendingBuff, 0, MAX);
	string success = "Successfully Registered";
	strcpy(sendingBuff, success.c_str());
	write(new_socket, sendingBuff, (int)MAX);

	// release the lock on the mutex
	mtx.unlock();
}
/*
 * change password method
 */
void Server::changePassword(int new_socket, int id) {
	// Lock the mutex to prevent other threads from modifying shared data
	mtx.lock();

	// Clear buffers to prevent any previous data from being read or written
	memset(sendingBuff, 0, MAX);

	// Send message to client requesting new password
	string pWord = " Please insert new password:\n";
	strcpy(sendingBuff, pWord.c_str());
	write(new_socket, sendingBuff, (int)MAX);

	// Clear the receiving buffer and read new password from client
	memset(receivingBuff, 0, MAX);
	read(new_socket, receivingBuff, (size_t)MAX);
	string newPassword = receivingBuff;

	string uName;
	// Find the user with the specified ID and update their password
	for (unsigned i = 0; i < users.size(); i++)
	{
		if (users[i].getId() == id)
		{
			uName = users[i].getUsername();
			users[i].setPassword(newPassword);
			break;
		}
	}

	string username;
	string password;
	string inLine;

	// Open the users file for reading and a temporary file for writing
	ifstream usersFile;
	fstream tempFile;
	usersFile.open("users.txt", ios::in);
	tempFile.open("temp.txt", ios_base::app | ios::out | ios::in);

	// If both files are open, read each line from the users file
	// and write it to the temp file with the updated password
	if (usersFile.is_open() && tempFile.is_open()) {
		while (getline(usersFile, inLine)) {
			stringstream ss(inLine);
			ss >> username;
			ss >> password;
			string newLine = username;
			newLine += " ";
			if (username == uName){
				newLine += newPassword;
			}else{
				newLine += password;
			}
			newLine += "\n";

			tempFile << newLine;
		}
	}

	// Close both files
	usersFile.close();
	tempFile.close();

	// Rename the temp file to the original file
	rename("temp.txt", "users.txt");

	// Send message to client indicating success
	memset(sendingBuff, 0, MAX);
	pWord = " Password changed successfully\n";
	strcpy(sendingBuff, pWord.c_str());
	write(new_socket, sendingBuff, (int)MAX);

	// Call the optionsWhenLoggedIn function to display the user options menu
	optionsWhenLoggedIn(new_socket);

	// Unlock the mutex to allow other threads to modify shared data
	mtx.unlock();
}


bool Server::mainMenu(int new_socket, int id) {
	memset(sendingBuff, 0, MAX);
	string firstOptions = "Welcome!\n Press 1 to Login\n  Press 2 to Register\n  Type \'exit\' to Quit\n";
	strcpy(sendingBuff, firstOptions.c_str());
	write(new_socket, sendingBuff, (int)MAX);

	while(loggedIn == false){
		memset(receivingBuff, 0, MAX);
		read(new_socket, receivingBuff, (size_t)MAX);
		if(strcmp(receivingBuff, "exit") == 0){
			exitProgram(new_socket, id);
			return false;
		}
		if(strcmp(receivingBuff, "1") == 0){
			if(Login(new_socket, id)){
				optionsWhenLoggedIn(new_socket);
				loggedIn = true;
			}else{
				string notFound = "Try again. Incorrect username or password.";
				strcpy(sendingBuff, notFound.c_str());
				write(new_socket, sendingBuff, (int)MAX);
				mainMenu(new_socket, id);
			}
		}
		if(strcmp(receivingBuff, "2") == 0){
			Register(new_socket);
			mainMenu(new_socket, id);
		}
	}
	return true;
}
/*
 * checkLogin method, checks to see if account exists.
 */
bool Server::checkLogin(string username, string password) {
	// Initializing variables to store the username and password read from the file.
	string uName, pWord;

	// Opening the "users.txt" file.
	ifstream usersFile;
	usersFile.open("users.txt");

	// If the file is open, read the username and password from the file line by line.
	if (usersFile.is_open())
	{
		usersFile >> uName >> pWord;

		// Continue reading until the end of the file is reached.
		while (!usersFile.eof()) {

			// If the username and password match, return true.
			if(uName == username && pWord == password){
				return true;
			}

			// Read the next line of the file.
			usersFile >> uName >> pWord;
		}

		// Close the file.
		usersFile.close();
	}

	// If the username and password were not found, return false.
	return false;
}

/*
 * optionsWhenLoggedIn, second menu for user, sub, unsub, view all, change password, exit
 */
void Server::optionsWhenLoggedIn(int new_socket) {
	memset(sendingBuff, 0, MAX);
	string options;
	options = " 1. Subscribe to a location\n";
	options += " 2. Unsubscribe to a location\n";
	options += " 3. See all the locations that the client has subscribed to\n";
	options += " 4. Change password\n";
	options += " 5. Exit\n";
	strcpy(sendingBuff, options.c_str());
	write(new_socket, sendingBuff, (int)MAX);

}
/*
 * unsubscribe method, unsubcribe location that a user has previously subscribed to.
 */
void Server::unsubscribe(int new_socket, int id){
	memset(sendingBuff, 0, MAX);
	string showLocations;
	for(std::vector<User>::size_type i = 0; i < users.size(); i++){
		if(users[i].getId() == id){
			showLocations += users[i].seeLocations();
		}
	}
	strcpy(sendingBuff, showLocations.c_str());
	write(new_socket, sendingBuff, (int)MAX);

	memset(sendingBuff, 0, MAX);
	string locToSub = "Please enter a location to unsubscribe";
	strcpy(sendingBuff, locToSub.c_str());
	write(new_socket, sendingBuff, (int)MAX);

	memset(receivingBuff, 0, MAX);
	read(new_socket, receivingBuff, (size_t)MAX);

	string location = receivingBuff;
	for(std::vector<User>::size_type i = 0; i < users.size(); i++){
		if(users[i].getId() == id){
			if(users[i].unsubscribe(location)){
				memset(sendingBuff, 0, MAX);
				locToSub = "Unsubscribed successfully";
				strcpy(sendingBuff, locToSub.c_str());
				write(new_socket, sendingBuff, (int)MAX);
				optionsWhenLoggedIn(new_socket);
			}else{
				memset(sendingBuff, 0, MAX);
				locToSub = "Could not find the location";
				strcpy(sendingBuff, locToSub.c_str());
				write(new_socket, sendingBuff, (int)MAX);
				optionsWhenLoggedIn(new_socket);
			}
		}
	}
}
/*
 * subscribe method, add locations to users account.
 */
void Server::subscribe(int new_socket, int id){
	memset(sendingBuff, 0, MAX);
	string locToSub = "Please enter a location you want to subscribe to:";
	strcpy(sendingBuff, locToSub.c_str());
	write(new_socket, sendingBuff, (int)MAX);

	memset(receivingBuff, 0, MAX);
	read(new_socket, receivingBuff, (size_t)MAX);

	string location = receivingBuff;

	for(std::vector<User>::size_type i = 0; i < users.size(); i++){
		if(users[i].getId() == id){
			users[i].subscribe(location);
		}
	}

	memset(sendingBuff, 0, MAX);
	locToSub = "Successfully Subscribed. Select an option";
	strcpy(sendingBuff, locToSub.c_str());
	write(new_socket, sendingBuff, (int)MAX);
	optionsWhenLoggedIn(new_socket);
}

/*
 * seeLocations method, displays added location that user has subscribed to.
 */
void Server::seeLocations(int new_socket, int id){
	// Initializing the sending buffer and the string that will contain the locations.
	memset(sendingBuff, 0, MAX);
	string locations = "";

	// Looping through the vector of users to find the user with the given id.
	for(int i = 0; i < static_cast<int>(users.size()); i++){
		if(users[i].getId() == id){
			// If the user is found, add their locations to the locations string.
			locations += users[i].seeLocations();
		}
	}

	// Copying the locations string to the sending buffer and sending it to the client.
	strcpy(sendingBuff, locations.c_str());
	write(new_socket, sendingBuff, (int)MAX);

	// Displaying the options for the user when they are logged in.
	optionsWhenLoggedIn(new_socket);

	/*
	 * exit feature method, successfully exits client from server.
	 */
}
void Server::exitProgram(int new_socket, int id){
	memset(sendingBuff, 0, MAX);
	string message = "exit";
	strcpy(sendingBuff, message.c_str());
	write(new_socket, sendingBuff, (int)MAX);
	close(new_socket);
	loggedIn = false;
	for (size_t i = 0; i < users.size(); i++)
	{
		if (users[i].getId() == id)
		{
			users.erase(users.begin() + i);
			break;
		}
	}
	usersActive--;
	cout << "User " << id << " has exited the server." << endl;
}

