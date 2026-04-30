CC = gcc
CFLAGS = -Wall -Wextra -Icommon
PTHREAD = -lpthread

CLIENT_SRC = $(wildcard client/*.c)
SERVER_SRC = $(wildcard server/*.c)

CLIENT_OBJ = $(patsubst %.c, %.o, $(CLIENT_SRC))
SERVER_OBJ = $(patsubst %.c, %.o, $(SERVER_SRC))

all: client_exec server_exec

client_exec: $(CLIENT_OBJ)
	$(CC) $(CFLAGS) $^ -o $@

server_exec: $(SERVER_OBJ)
	$(CC) $(CFLAGS) $^ -o $@ $(PTHREAD)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# 🔹 Run server
run_server: server_exec
	./server_exec

# 🔹 Run client
run_client: client_exec
	./client_exec

# 🔹 Run both (server in background + client)
run: server_exec client_exec
	./server_exec & \
	sleep 1 && \
	./client_exec

clean:
	rm -f client/*.o server/*.o client_exec server_exec

rebuild: clean all