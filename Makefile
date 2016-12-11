SERVER_TGT = guess-server
SERVER_SRCS = io.c numbers.c server.c
CLIENT_TGT = guess-client
CLIENT_SRCS = io.c client.c
CFLAGS = -Wall -Wextra --std=gnu99 -g -O2 -MD -MP

all: $(SERVER_TGT) $(CLIENT_TGT)
.PHONY: all

$(SERVER_TGT): $(SERVER_SRCS:%.c=%.o)
	$(CC) $(CFLAGS) -o $@ $^

$(CLIENT_TGT): $(CLIENT_SRCS:%.c=%.o)
	$(CC) $(CFLAGS) -o $@ $^

.PHONY: clean
clean:
	rm -f $(SERVER_TGT) $(CLIENT_TGT) *.o *.d

-include *.d
