/*
 *
 */

#include "server.hpp"

Server::Server() {
	id = 0;
    	usersActive = 0;
    	addrLen = sizeof(address);
    	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
		perror("ERROR: first socket if statement");
		exit(EXIT_FAILURE);
	}
    	address.sin_family = AF_INET;
    	address.sin_addr.s_addr = INADDR_ANY;
    	address.sin_port = htons(PORT);
    	memset(address.sin_zero, '\0', sizeof address.sin_zero);
    	if (bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0) {
		perror("ERROR: bind if statement");
		exit(EXIT_FAILURE);
    	}
    	cout << "bind done" << endl;
    	if (listen(server_fd, 10) < 0) {
		perror("ERROR: listen if statement");
		exit(EXIT_FAILURE);
    	}


    while(true) {
        printf("Waiting for incoming connections...\n");
        if((new_socket = accept(server_fd, (struct sockaddr *) &address, (socklen_t *) &addrLen)) < 0){
            perror("ERROR: while loop, accept connections if statement");
            exit(EXIT_FAILURE);
        }
        id++;
        threads.push_back(std::thread(&Server::run, this, new_socket, id));


    }
    for (size_t i = 0; i < threads.size(); i++)
    {
        if (threads[i].joinable())
            threads[i].join();
    }
}

void Server::run(int new_socket, int id){
    cout << "Connection accepted" << endl;
    cout << "Handler assigned" << endl;
    cout << "ID #" << id << endl;
    bool connected = true;
    connected = mainMenu(new_socket, id);

    while(loggedIn){
        memset(receivingBuff, 0, MAX);
        read(new_socket, receivingBuff, (size_t)MAX);
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
            connected = mainMenu(new_socket, id);
        }
	else if(strcmp(receivingBuff, "0") == 0) {
		exitProgram(new_socket, id);
        return;
	}

	else{
            memset(sendingBuff, 0, MAX);
            string invalid = "Invalid choice, please try again";
            strcpy(sendingBuff, invalid.c_str());
            write(new_socket, sendingBuff, (int)MAX);
            optionsWhenLoggedIn(new_socket);
        }
    }
    if(!connected){
        return;
    }
}


bool Server::Login(int new_socket, int id) {
    memset(receivingBuff, 0, MAX);
    memset(sendingBuff, 0, MAX);

    string username, password;
    string uName = " Username:\n";

    strcpy(sendingBuff, uName.c_str());
    write(new_socket, sendingBuff, (int)MAX);
    memset(receivingBuff, 0, MAX);
    read(new_socket, receivingBuff, (size_t)MAX);

    username = receivingBuff;
    memset(sendingBuff, 0, MAX);
    string pWord = " Password:\n";
    strcpy(sendingBuff, pWord.c_str());
    write(new_socket, sendingBuff, (int)MAX);
    read(new_socket, receivingBuff, (size_t)MAX);
    password = receivingBuff;
    cout << "u: " << username << " p: "  << password << endl;

    if(checkLogin(username, password)){
        User userr(username, password, new_socket, id);
        users.push_back(userr);
        usersActive++;
        cout << "Users active: " << usersActive << endl;
        memset(sendingBuff, 0, MAX);
        string logged = "Successfully Logged In";
        strcpy(sendingBuff, logged.c_str());
        write(new_socket, sendingBuff, (int)MAX);
        return true;
    }
    else{
        return false;
    }

}


void Server::Register(int new_socket){
    mtx.lock();
    memset(receivingBuff, 0, MAX);
    memset(sendingBuff, 0, MAX);

    string username, password;
    string uName = " Username:\n";

    strcpy(sendingBuff, uName.c_str());
    write(new_socket, sendingBuff, (int)MAX);
    memset(receivingBuff, 0, MAX);
    read(new_socket, receivingBuff, (size_t)MAX);

    username = receivingBuff;
    memset(sendingBuff, 0, MAX);
    string pWord = " Password:\n";
    strcpy(sendingBuff, pWord.c_str());
    write(new_socket, sendingBuff, (int)MAX);
    read(new_socket, receivingBuff, (size_t)MAX);
    password = receivingBuff;
    cout << "u: " << username << " p: "  << password << endl;

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
    memset(sendingBuff, 0, MAX);
    string success = "Successfully Registered";
    strcpy(sendingBuff, success.c_str());
    write(new_socket, sendingBuff, (int)MAX);
    mtx.unlock();
}

void Server::changePassword(int new_socket, int id) {
    mtx.lock();
    memset(sendingBuff, 0, MAX);
    string pWord = " Please insert new password:\n";
    strcpy(sendingBuff, pWord.c_str());
    write(new_socket, sendingBuff, (int)MAX);

    memset(receivingBuff, 0, MAX);
    read(new_socket, receivingBuff, (size_t)MAX);
    string newPassword = receivingBuff;

    string uName;
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

    ifstream usersFile;
    fstream tempFile;
    usersFile.open("users.txt", ios::in);
    tempFile.open("temp.txt", ios_base::app | ios::out | ios::in);
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

    usersFile.close();
    tempFile.close();
    rename("temp.txt", "users.txt");

    memset(sendingBuff, 0, MAX);
    pWord = " Password changed successfully\n";
    strcpy(sendingBuff, pWord.c_str());
    write(new_socket, sendingBuff, (int)MAX);
    optionsWhenLoggedIn(new_socket);
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

bool Server::checkLogin(string username, string password) {
    string uName, pWord;
    ifstream usersFile;
    usersFile.open("users.txt");
    if (usersFile.is_open())
    {
        usersFile >> uName >> pWord;
        while (!usersFile.eof()) {
            if(uName == username && pWord == password){
                return true;
            }
            usersFile >> uName >> pWord;
        }
        usersFile.close();
    }
    return false;
}

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

void Server::seeLocations(int new_socket, int id){
    memset(sendingBuff, 0, MAX);
    string locations = "";
    for(int i = 0; i < static_cast<int>(users.size()); i++){
        if(users[i].getId() == id){
            locations += users[i].seeLocations();
        }
    }
    strcpy(sendingBuff, locations.c_str());
    write(new_socket, sendingBuff, (int)MAX);
    optionsWhenLoggedIn(new_socket);
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
    //exit(EXIT_SUCCESS);
}

