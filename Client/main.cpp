/*
 *
 */

#include "client.hpp"

void sendMsg(int client_fd);
void receiveMsg(int client_fd);
bool exitClient = false;
int main(){

    Client c;
    string ipAddress;
    do{
        cout << "Enter hostname: ";
        cin >> ipAddress;
        if(ipAddress != "localhost"){
            cout << "Can't find the hostname, try again" << endl;
        }
    }while(ipAddress != "localhost");
    ipAddress = "127.0.0.1";

    if((c.client_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        perror("Could not make socket !!!");
        exit(EXIT_FAILURE);
    } else{
        cout << "Socket created" << endl;
    }

    memset(&c.address, '\0', sizeof c.address);
    c.address.sin_family = AF_INET;
    c.address.sin_port = htons(PORT);
    c.address.sin_addr.s_addr = inet_addr(ipAddress.c_str());

    if(connect(c.client_fd, (struct sockaddr*)&c.address, sizeof(c.address)) < 0){
        perror("Failed to connect !!!");
        close(c.client_fd);
        exit(EXIT_FAILURE);
    } else{
        cout << "localhost resolved to " << ipAddress << endl << "Connected" << endl << endl;
    }

    memset(c.sendingBuff, '\0', sizeof c.sendingBuff);
    memset(c.receivingBuff, '\0', sizeof c.receivingBuff);

    read(c.client_fd, c.receivingBuff, MAX);
    cout << c.receivingBuff << endl;
    memset(c.sendingBuff,0,MAX);
    cin.getline(c.sendingBuff, MAX);
    write(c.client_fd, c.sendingBuff, MAX);

    thread t1(receiveMsg, c.client_fd);
    thread t2(sendMsg, c.client_fd);

    c.threadReceiving = move(t1);
    c.threadSending = move(t2);


    //if (c.threadReceiving.joinable())
        c.threadReceiving.join();
        cout << "Joined Recieving" << endl;

    //if (c.threadSending.joinable())
    exit(EXIT_SUCCESS);
        //c.threadSending.join();
        cout << "Joined Sending" << endl;


    return 0;
}

void receiveMsg(int client_fd) {
    Client c;
    while(true){
        
        memset(c.receivingBuff, 0, MAX);
        int bytesReceived = read(client_fd, c.receivingBuff, MAX);
        if(bytesReceived <= 0){
            continue;
        }
        
        
        cout << c.receivingBuff << endl;
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

