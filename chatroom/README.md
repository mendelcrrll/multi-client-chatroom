# Multi-Client Chatroom Application

A C++ multi-threaded chatroom application that allows multiple users to connect, authenticate, and chat in real-time.

## Features

- User authentication via username and password
- Multi-threaded server to handle multiple client connections simultaneously
- Real-time message broadcasting to all connected users
- User join/leave notifications
- Thread-safe user management

## Building

The application uses a Makefile for easy compilation. Simply run:

```bash
cd /homes/iws/mcarro5/CSE333TA/multi-client-chatroom/chatroom
make clean
make
```

This will compile:
- `server` - The chatroom server executable
- `client` - The chatroom client executable

## Running the Chatroom

### Starting the Server

The server listens on a specified port and loads user credentials from a JSON file:

```bash
./server <port> <credentials_file>
```

Example:
```bash
./server 8888 mock_users.json
```

The server will output:
```
Loaded 5 users from credentials file
Server listening on port 8888
Available users: alice, bob, charlie, dana, eve
```

### Starting a Client

Connect to the server from a different terminal:

```bash
./client <host> <port>
```

Example:
```bash
./client localhost 8888
```

## Using the Chatroom

After connecting, you'll see:
```
Connecting to localhost:8888...
Connected to server!
Commands: JOIN <username> <password>, MSG <message>, LEAVE, QUIT
```

### Available Commands

1. **JOIN** - Authenticate and join the chatroom
   ```
   JOIN alice password123
   ```

2. **MSG** - Send a message to all users in the chatroom
   ```
   MSG Hello everyone!
   ```

3. **LEAVE** - Leave the chatroom gracefully
   ```
   LEAVE
   ```

4. **QUIT** - Disconnect from the server and exit
   ```
   QUIT
   ```

## Default Credentials

The application comes with 5 pre-loaded users in `mock_users.json`:

| Username | Password    |
|----------|------------|
| alice    | password123|
| bob      | hunter2    |
| charlie  | charlie_pw |
| dana     | letmein    |
| eve      | eve_secret |

## Testing the Application

### Single Client Test
```bash
# Terminal 1: Start server
./server 8888 mock_users.json

# Terminal 2: Start client
./client localhost 8888
# Then type: JOIN alice password123
# Then type: MSG Hello world!
# Then type: QUIT
```

### Multi-Client Chat Test
```bash
# Terminal 1: Start server
./server 8888 mock_users.json

# Terminal 2: Start first client (alice)
./client localhost 8888
# Type: JOIN alice password123
# Type: MSG Hi from Alice

# Terminal 3: Start second client (bob)
./client localhost 8888
# Type: JOIN bob hunter2
# Type: MSG Hi from Bob!

# Messages sent by either client will be broadcast to all others
```

## Architecture

### Server Components
- `Server::listenServer()` - Sets up the listening socket
- `Server::joinUser()` - Authenticates and registers users
- `Server::broadcast()` - Sends messages to all connected users
- `Server::handleMessage()` - Processes client commands
- `Server::leaveUser()` - Handles user disconnections

### Client Components
- `Client::connect()` - Establishes connection to server
- `Client::sendMessage()` - Sends commands/messages to server
- `Client::receiveMessage()` - Listens for incoming messages
- Main loop handles user input and maintains connection

### User Management
- `User` class stores user information (name, password, status, session info)
- Credentials loaded from JSON file using nlohmann/json library
- Active users tracked in unordered_map with file descriptor as key

## Technical Details

- **Language**: C++17
- **Threading**: `std::thread` for multi-client handling
- **Networking**: BSD sockets (IPv6 with IPv4 mapping)
- **JSON Parsing**: nlohmann/json library
- **Build System**: Makefile with g++ compiler

## Cleanup

To remove compiled objects and executables:

```bash
make clean
```

## Future Enhancements

- User management (registration, password change)
- Private messaging between users
- Chat rooms/channels
- User presence indicators
- Message history persistence
- SSL/TLS encryption
- Better error handling and logging
