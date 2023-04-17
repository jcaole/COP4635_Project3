#ifndef USER_HPP
#define USER_HPP
#include<iostream>
#include<string>
#include<map>
#include<fstream>
#include <algorithm>
#include <vector>

using namespace std;

class User{

    public:

        User();
        User(string username, string password, int new_socket, int id);
        string username;
        string password;
        int new_socket;
        int id;
        string getUsername();
        string getPassword();
        int getNewSocket();
        int getId();
        void setUsername(string username);
        void setPassword(string password);
        void setNewSocket(int new_socket);
        void setId(int id);
        void subscribe(string location);
        bool unsubscribe(string location);
        string seeLocations();
        vector<string> getLocations();
        vector<string> locations;
        bool isActive = false;
};


#endif //USER_HPP
