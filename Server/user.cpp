/*
 *
 */
#include "user.hpp"

/*
 * param constructor, initialize user, param username, password, socket, and ID
 */
User::User(string username, string password, int new_socket, int id) {
	this->username = username;
	this->password = password;
	this->new_socket = new_socket;
	this->id = id;
	this->locations = {};
}

/*
 *  Default constructor
 */
User::User(){
	this->username = "";
	this->password = "";
	this->locations = {};
}
// setters
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

// needed for subscribe feature, subscribe to location
void User::subscribe(string location){
	this->locations.push_back(location);
}

// getters
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
vector<string> User::getLocations() {
	return locations;
}

/*
 *  Method to unsubscribe from a location
 */
bool User::unsubscribe(string location) {
	// Find location in  vector
	auto it = find(locations.begin(), locations.end(), location);
	// If the location is not found, return false
	if (it == locations.end()) {
		return false;
	}
	// If the location is found, remove it from the vector and return true
	else {
		int index = std::distance(locations.begin(), it);
		locations.erase(locations.begin() + index);
		return true;
	}
}

/*
 *  seeLocation method, see the locations subscribed by the user
 */
string User::seeLocations() {
	string loc;
	// Iterate through the locations vector and create a string with the location and index
	for (unsigned int i = 0; i < locations.size(); i++) {
		loc += to_string(i) + ": "+ locations[i] + "\n";
	}
	return loc;
}

