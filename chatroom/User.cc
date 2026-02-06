#include "User.h"

#include <chrono>
#include <ostream>

namespace ChatroomUser {

// --------------------
// Non-member operators
// --------------------

bool operator==(const User& lhs, const User& rhs) {
  // Users are equal if they have the same immutable ID
  return lhs.user_id_ == rhs.user_id_;
}

std::ostream& operator<<(std::ostream& os, const User& user) {
  os << "User{id=" << user.user_id_
     << ", name=" << user.user_name_
     << ", status=";

  switch (user.status_) {
    case User::UserStatus::Online:
      os << "Online";
      break;
    case User::UserStatus::Away:
      os << "Away";
      break;
    case User::UserStatus::Offline:
      os << "Offline";
      break;
  }

  return os << "}";
}

}  // namespace ChatroomUser