#include "Server.h"

#include <arpa/inet.h>
#include <assert.h>
#include <errno.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <iostream>
#include <sstream>
#include <sys/socket.h>
#include <unistd.h>
#include <cstring>
#include <cerrno>
#include <fstream>
#include <nlohmann/json.hpp>

using namespace ChatroomUser;
using namespace std;
using json = nlohmann::json;

namespace ChatroomServer {

  // Helper function for sending user options on request
  void userOptions();

  bool Server::loadCredentials(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
      std::cerr << "Error opening credentials file: " << filename << std::endl;
      return false;
    }

    try {
      json data;
      file >> data;

      int user_id = 1;
      for (const auto& user_obj : data["users"]) {
        User user(user_obj["username"].get<std::string>(), user_id++);
        user.setPassword(user_obj["password"].get<std::string>());
        user_database_.push_back(user);
      }
      std::cout << "Loaded " << user_database_.size() << " users from credentials file" << std::endl;
      return true;
    } catch (const std::exception& e) {
      std::cerr << "Error parsing JSON: " << e.what() << std::endl;
      return false;
    }
  }

  bool Server::authenticateUser(const std::string& username,
                                const std::string& password) {
    for (const auto& user : user_database_) {
      if (user.getName() == username && user.getPassword() == password) {
        return true;
      }
    }
    return false;
  }

  bool Server::joinUser(const string& username, const string& password, int client_fd) {
    // Check if user is already connected
    for (const auto& [fd, user] : active_users_) {
      if (user.getName() == username) {
        send(client_fd, "ERR user already connected\n", 28, 0);
        return false;
      }
    }

    // Authenticate user
    if (!authenticateUser(username, password)) {
      send(client_fd, "ERR authentication failed\n", 26, 0);
      return false;
    }

    // Find the user in database and add to active users
    for (const auto& user : user_database_) {
      if (user.getName() == username) {
        User active_user = user;
        active_user.setStatus(User::UserStatus::Online);
        active_users_[client_fd] = active_user;
        send(client_fd, "OK\n", 3, 0);
        return true;
      }
    }

    return false;
  }

  void Server::leaveUser(int client_fd) {
    auto it = active_users_.find(client_fd);
    if (it == active_users_.end()) {
      return;
    }

    const std::string exit_msg =
        it->second.getName() + " has left the chat.\n";

    active_users_.erase(it);
    broadcastToAllIncludingSender(exit_msg);
    close(client_fd);
  }

  void Server::handleMessage(int client_fd, const std::string& message) {

    std::istringstream iss(message);
    std::string command;
    iss >> command;

    if (command == "JOIN") {
      std::string username, password;
      iss >> username;
      iss >> password;

      if (username.empty()) {
        send(client_fd, "ERR missing username\n", 21, 0);
        return;
      }

      if (!joinUser(username, password, client_fd)) {
        return;
      }

      std::string notice = username + " joined the chat\n";
      broadcastToAllIncludingSender(notice);
      return;
    }

    if (command == "MSG") {
      std::string text;
      std::getline(iss, text);

      if (text.empty()) {
        send(client_fd, "ERR empty message\n", 18, 0);
        return;
      }

      // Remove leading space from getline
      if (!text.empty() && text[0] == ' ') {
        text.erase(0, 1);
      }

      // Get username and format message with sender info
      auto it = active_users_.find(client_fd);
      if (it != active_users_.end()) {
        std::string formatted_msg = it->second.getName() + ": " + text + "\n";
        // Broadcast to all users including sender
        broadcastToAll(client_fd, formatted_msg);
      }
      return;
    }

    if (command == "LEAVE") {
      leaveUser(client_fd);
      return;
    }

    if (command == "QUIT") {
      leaveUser(client_fd);
      close(client_fd);
      return;
    }

    if (command == "OPTIONS") {
      // Some logic
    }
    

    // Unknown command
    send(client_fd, "ERR unknown command\n", 21, 0);
  }
  
  void Server::broadcast(const int client_fd, const std::string& message) {
    broadcastToAll(client_fd, message);
  }

  void Server::broadcastToAll(const int sender_fd, const std::string& message) {
    for (const auto& [fd, user] : active_users_) {
      if (fd == sender_fd) {
        continue; // Skip sender if specified
      }
      ssize_t total_sent = 0;
      ssize_t to_send = message.size();

      while (total_sent < to_send) {
        ssize_t n = send(fd,
                        message.data() + total_sent,
                        to_send - total_sent,
                        0);

        if (n <= 0) {
          // Client likely disconnected or errored
          // For now, just stop sending to this client
          break;
        }

        total_sent += n;
      }
    }
  }

  void Server::broadcastToAllIncludingSender(const std::string& message) {
    for (const auto& [fd, user] : active_users_) {
      ssize_t total_sent = 0;
      ssize_t to_send = message.size();

      while (total_sent < to_send) {
        ssize_t n = send(fd,
                        message.data() + total_sent,
                        to_send - total_sent,
                        0);

        if (n <= 0) {
          // Client likely disconnected or errored
          // For now, just stop sending to this client
          break;
        }

        total_sent += n;
      }
    }
  }


  int Server::listenServer(const char* portnum, int* sock_family) {
    // Populate the "hints" addrinfo structure for getaddrinfo().
    // ("man addrinfo")
    struct addrinfo hints;
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET6;       // IPv6 (also handles IPv4 clients)
    hints.ai_socktype = SOCK_STREAM;  // stream
    hints.ai_flags = AI_PASSIVE;      // use wildcard "in6addr_any" address
    hints.ai_flags |= AI_V4MAPPED;    // use v4-mapped v6 if no v6 found
    hints.ai_protocol = IPPROTO_TCP;  // tcp protocol
    hints.ai_canonname = nullptr;
    hints.ai_addr = nullptr;
    hints.ai_next = nullptr;

    // Use argv[1] as the string representation of our portnumber to
    // pass in to getaddrinfo().  getaddrinfo() returns a list of
    // address structures via the output parameter "result".
    struct addrinfo *result;
    int res = getaddrinfo(nullptr, portnum, &hints, &result);

    // Did addrinfo() fail?
    if (res != 0) {
      std::cerr << "getaddrinfo() failed: ";
      std::cerr << gai_strerror(res) << std::endl;
      return -1;
    }

    // Loop through the returned address structures until we are able
    // to create a socket and bind to one.  The address structures are
    // linked in a list through the "ai_next" field of result.
    int listen_fd = -1;
    for (struct addrinfo *rp = result; rp != nullptr; rp = rp->ai_next) {
      listen_fd = socket(rp->ai_family,
                        rp->ai_socktype,
                        rp->ai_protocol);
      if (listen_fd == -1) {
        // Creating this socket failed.  So, loop to the next returned
        // result and try again.
        std::cerr << "socket() failed: " << strerror(errno) << std::endl;
        listen_fd = -1;
        continue;
      }

      // Configure the socket; we're setting a socket "option."  In
      // particular, we set "SO_REUSEADDR", which tells the TCP stack
      // so make the port we bind to available again as soon as we
      // exit, rather than waiting for a few tens of seconds to recycle it.
      int optval = 1;
      setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR,
                &optval, sizeof(optval));

      // Try binding the socket to the address and port number returned
      // by getaddrinfo().
      if (bind(listen_fd, rp->ai_addr, rp->ai_addrlen) == 0) {
        // Bind worked!

        // Return to the caller the address family.
        *sock_family = rp->ai_family;
        break;
      }

      // The bind failed.  Close the socket, then loop back around and
      // try the next address/port returned by getaddrinfo().
      close(listen_fd);
      listen_fd = -1;
    }

    // Free the structure returned by getaddrinfo().
    freeaddrinfo(result);

    // If we failed to bind, return failure.
    if (listen_fd == -1)
      return listen_fd;

    // Success. Tell the OS that we want this to be a listening socket.
    if (listen(listen_fd, SOMAXCONN) != 0) {
      std::cerr << "Failed to mark socket as listening: ";
      std::cerr << strerror(errno) << std::endl;
      close(listen_fd);
      return -1;
    }

    // Return to the client the listening file descriptor.
    return listen_fd;
  }
}