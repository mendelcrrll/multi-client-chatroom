#ifndef CLIENT_H
#define CLIENT_H

#include <string>
#include <memory>

class Client {
public:
    Client(const std::string& host, int port);
    ~Client();
    
    bool connect();
    void disconnect();
    bool isConnected() const;
    
    bool sendMessage(const std::string& message);
    std::string receiveMessage();
    
    void setUsername(const std::string& username);
    std::string getUsername() const;
    
private:
    std::string host_;
    int port_;
    int socketFd_;
    std::string username_;
    bool connected_;
    
    void closeSocket();
};

#endif // CLIENT_H