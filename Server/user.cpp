/*
 *
 */

#include "user.hpp"

User::User(string username, string password, int new_socket, int id) {
	this->username = username;
    	this->password = password;
    	this->new_socket = new_socket;
    	this->id = id;
    	this->locations = {};
}

User::User(){
    	this->username = "";
    	this->password = "";
    	this->locations = {};
}

string User::getUsername(){
    	return username;
}

string User::getPassword(){
    	return password;
}

int User::getNewSocket() {
    	return new_socket;
}

int User::getId(){
    	return id;
}

void User::setUsername(string username){
    	this->username = username;
}

void User::setPassword(string password) {
    	this->password = password;
}

void User::setNewSocket(int new_socket) {
    	this->new_socket = new_socket;
}

void User::setId(int id) {
    	this->id = id;
}

void User::subscribe(string location){
    	this->locations.push_back(location);
}

vector<string> User::getLocations() {
    	return locations;
}

bool User::unsubscribe(string location) {
    	auto it = find(locations.begin(), locations.end(), location);
    	if (it == locations.end()) {
		return false;
    	}
       	else {
		int index = std::distance(locations.begin(), it);
		locations.erase(locations.begin() + index);
		return true;
    	}
}

string User::seeLocations() {
    	string loc;
    	for (unsigned int i = 0; i < locations.size(); i++) {
		loc += to_string(i) + ": "+ locations[i] + "\n";
    	}
    	return loc;
}
