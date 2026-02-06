#ifndef SERVER_H_
#define SERVER_H_

#include <string>
#include <unordered_map>
#include <vector>

#include "User.h"

namespace ChatroomServer {

class Server {
 public:
  // ---- Constructor ----
  explicit Server(int max_clients) : max_clients_(max_clients) {}

  ~Server() = default;

  // Disable copy constructor
  Server(const Server&) = delete;

  /**
   * @brief Load user credentials from a JSON file
   */
  bool loadCredentials(const std::string& filename);

  /**
   * @brief Authenticate a user by username/password
   */
  bool authenticateUser(const std::string& username,
                        const std::string& password);

  /**
   * @brief Join a user to the chatroom
   */
  bool joinUser(const std::string& username, const std::string& password, int client_fd);

  /**
   * @brief Remove a user from the chatroom
   */
  void leaveUser(int client_fd);

  /**
   * @brief Handle a message received from a client
   */
  void handleMessage(int client_fd, const std::string& message);

  /**
   * @brief Broadcast a message to all connected clients
   */
  void broadcast(int sender_fd, const std::string& message);

  /**
   * @brief Broadcast a message to all connected clients excluding sender
   */
  void broadcastToAll(int sender_fd, const std::string& message);

  /**
   * @brief Broadcast a message to all connected clients including sender
   */
  void broadcastToAllIncludingSender(const std::string& message);

  /**
   * @brief Create and bind a listening socket
   */
  int listenServer(const char* portnum, int* sock_family);

  // Getters
  int getMaxClients() const { return max_clients_; }
  const std::unordered_map<int, ChatroomUser::User>&
  getUsers() const {
    return active_users_;
  }

 private:
  const int max_clients_;

  // fd (socket) -> active user
  std::unordered_map<int, ChatroomUser::User> active_users_;

  // credential database
  std::vector<ChatroomUser::User> user_database_;
};

}  // namespace ChatroomServer

#endif  // SERVER_H_