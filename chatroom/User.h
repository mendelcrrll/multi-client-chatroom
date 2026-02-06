#ifndef USER_H_
#define USER_H_

#include <string>
#include <chrono>
#include <ostream>

namespace ChatroomUser {

class User {
 public:
  // ---- Types ----
  enum class UserStatus { Online, Away, Offline };

  using TimePoint = std::chrono::system_clock::time_point;

  // ---- Constructor / Destructor ----
  User() : user_id_(-1), user_name_(""), status_(UserStatus::Offline) {}

  User(const std::string& user_name, int id)
      : user_id_(id),
        user_name_(user_name),
        status_(UserStatus::Offline) {}

  ~User() = default;

  // ---- Getters ----
  int getId() const { return user_id_; }

  const std::string& getName() const { return user_name_; }
  const std::string& getPassword() const { return password_; }

  UserStatus getStatus() const { return status_; }

  TimePoint getActiveSessionStartTime() const {
    return active_session_start_time_;
  }

  TimePoint getLastActiveSession() const {
    return last_active_session_;
  }

  // ---- Setters ----
  void setUserName(const std::string& name) {
    user_name_ = name;
  }

  void setPassword(const std::string& password) {
    password_ = password;
  }

  void setStatus(UserStatus status) {
    status_ = status;
  }

  void setActiveSessionStartTime(TimePoint t) {
    active_session_start_time_ = t;
  }

  void setLastActiveSession(TimePoint t) {
    last_active_session_ = t;
  }

 private:
  // ---- Data members ----
  int user_id_;
  std::string user_name_;
  std::string password_;

  UserStatus status_;

  TimePoint active_session_start_time_{};
  TimePoint last_active_session_{};

  // ---- Non-member operators ----
  friend bool operator==(const User& lhs, const User& rhs);
  friend std::ostream& operator<<(std::ostream& os, const User& user);
};

}  // namespace ChatroomUser

#endif  // USER_H_