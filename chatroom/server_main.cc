#include "Server.h"

#include <iostream>
#include <thread>
#include <vector>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstring>
#include <cerrno>

using namespace ChatroomServer;
using namespace std;

Server* g_server = nullptr;

void handleClient(int client_fd) {
  char buffer[1024];
  
  while (true) {
    memset(buffer, 0, sizeof(buffer));
    ssize_t n = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
    
    if (n <= 0) {
      // Client disconnected
      g_server->leaveUser(client_fd);
      break;
    }
    
    std::string message(buffer);
    g_server->handleMessage(client_fd, message);
  }
}

int main(int argc, char* argv[]) {
  if (argc != 3) {
    std::cerr << "Usage: " << argv[0] << " <port> <credentials_file>" << std::endl;
    return 1;
  }

  const char* port = argv[1];
  const std::string credentials_file = argv[2];

  // Create server with max 10 clients
  g_server = new Server(10);

  // Load credentials from JSON file
  if (!g_server->loadCredentials(credentials_file)) {
    std::cerr << "Failed to load credentials" << std::endl;
    delete g_server;
    return 1;
  }

  int sock_family;
  int listen_fd = g_server->listenServer(port, &sock_family);
  
  if (listen_fd < 0) {
    std::cerr << "Failed to create listening socket" << std::endl;
    delete g_server;
    return 1;
  }

  std::cout << "Server listening on port " << port << std::endl;
  std::cout << "Available users: alice, bob, charlie, dana, eve" << std::endl;

  // Accept and handle connections
  while (true) {
    struct sockaddr_in6 clientAddr;
    socklen_t clientLen = sizeof(clientAddr);
    
    int client_fd = accept(listen_fd, (struct sockaddr*)&clientAddr, &clientLen);
    
    if (client_fd < 0) {
      std::cerr << "Error accepting connection: " << strerror(errno) << std::endl;
      continue;
    }

    std::cout << "New client connected" << std::endl;
    
    // Handle client in a separate thread
    std::thread client_thread(handleClient, client_fd);
    client_thread.detach();
  }

  close(listen_fd);
  delete g_server;
  return 0;
}
