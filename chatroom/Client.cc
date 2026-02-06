// filepath: /homes/iws/mcarro5/CSE333TA/multi-client-chatroom/chatroom/Client.cc

#include "Client.h"
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

Client::Client(const std::string& host, int port)
    : host_(host), port_(port), socketFd_(-1), username_(""), connected_(false) {}

Client::~Client() {
    disconnect();
}

bool Client::connect() {
    struct sockaddr_in serverAddr;
    struct hostent* server;
    
    socketFd_ = socket(AF_INET, SOCK_STREAM, 0);
    if (socketFd_ < 0) {
        std::cerr << "Error opening socket" << std::endl;
        return false;
    }
    
    server = gethostbyname(host_.c_str());
    if (server == nullptr) {
        std::cerr << "Error resolving host" << std::endl;
        closeSocket();
        return false;
    }
    
    std::memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port_);
    std::memcpy(&serverAddr.sin_addr.s_addr, server->h_addr, server->h_length);
    
    if (::connect(socketFd_, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        std::cerr << "Error connecting to server" << std::endl;
        closeSocket();
        return false;
    }
    
    connected_ = true;
    return true;
}

void Client::disconnect() {
    closeSocket();
    connected_ = false;
}

bool Client::isConnected() const {
    return connected_;
}

bool Client::sendMessage(const std::string& message) {
    if (!connected_) return false;
    
    ssize_t n = send(socketFd_, message.c_str(), message.length(), 0);
    return n > 0;
}

std::string Client::receiveMessage() {
    if (!connected_) return "";
    
    char buffer[1024] = {0};
    ssize_t n = recv(socketFd_, buffer, sizeof(buffer) - 1, 0);
    
    if (n <= 0) {
        connected_ = false;
        return "";
    }
    
    return std::string(buffer);
}

void Client::setUsername(const std::string& username) {
    username_ = username;
}

std::string Client::getUsername() const {
    return username_;
}

void Client::closeSocket() {
    if (socketFd_ >= 0) {
        close(socketFd_);
        socketFd_ = -1;
    }
}