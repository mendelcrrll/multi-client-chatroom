CXX := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -pthread
LDFLAGS := -pthread

# nlohmann/json is header-only, using local json.hpp
JSON_INCLUDE := -I.

SOURCES := Client.cc Server.cc User.cc
SERVER_MAIN := server_main.cc
CLIENT_MAIN := client_main.cc

OBJECTS := $(SOURCES:.cc=.o)
SERVER_OBJ := $(OBJECTS) $(SERVER_MAIN:.cc=.o)
CLIENT_OBJ := $(OBJECTS) $(CLIENT_MAIN:.cc=.o)

SERVER := server
CLIENT := client

.PHONY: all clean run-server run-client

all: $(SERVER) $(CLIENT)

$(SERVER): $(SERVER_OBJ)
	$(CXX) $(CXXFLAGS) $(JSON_INCLUDE) -o $@ $^ $(LDFLAGS)

$(CLIENT): $(CLIENT_OBJ)
	$(CXX) $(CXXFLAGS) $(JSON_INCLUDE) -o $@ $^ $(LDFLAGS)

%.o: %.cc
	$(CXX) $(CXXFLAGS) $(JSON_INCLUDE) -c $< -o $@

run-server: $(SERVER)
	./$(SERVER) 9999 mock_users.json

run-client: $(CLIENT)
	./$(CLIENT) localhost 9999

clean:
	rm -f $(OBJECTS) $(SERVER_MAIN:.cc=.o) $(CLIENT_MAIN:.cc=.o) $(SERVER) $(CLIENT)
