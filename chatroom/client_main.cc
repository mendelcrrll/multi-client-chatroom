#include "Client.h"

#include <iostream>
#include <thread>
#include <string>

using namespace std;

void receiveMessages(Client& client) {
  while (client.isConnected()) {
    string message = client.receiveMessage();
    if (!message.empty()) {
      cout << message << endl;
    }
  }
}

int main(int argc, char* argv[]) {
  if (argc != 3) {
    cerr << "Usage: " << argv[0] << " <host> <port>" << endl;
    return 1;
  }

  string host = argv[1];
  int port = stoi(argv[2]);

  Client client(host, port);

  cout << "Connecting to " << host << ":" << port << "..." << endl;
  
  if (!client.connect()) {
    cerr << "Failed to connect to server" << endl;
    return 1;
  }

  cout << "Connected to server!" << endl;
  cout << "Commands: JOIN <username> <password>, MSG <message>, LEAVE, QUIT" << endl;

  // Start receiving messages in a separate thread
  thread receive_thread(receiveMessages, ref(client));
  receive_thread.detach();

  // Main thread handles user input
  string line;
  while (getline(cin, line)) {
    if (line.empty()) {
      continue;
    }

    if (!client.sendMessage(line + "\n")) {
      cerr << "Failed to send message" << endl;
      break;
    }

    // Check if user wants to quit
    if (line.find("QUIT") == 0 || line.find("LEAVE") == 0) {
      break;
    }
  }

  client.disconnect();
  cout << "Disconnected from server" << endl;
  return 0;
}
