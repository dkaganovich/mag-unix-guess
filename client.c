#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <inttypes.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>

#include "io.h"


void print_usage(const char* prog) {
    fprintf(stderr, "=== number guessing client ===\n");
    fprintf(stderr, "Usage: %s UNIX_SOCKET_PATH\n\n", prog);
}

int main(int argc, char* argv[])
{
    if (argc != 2) {
        print_usage(argv[0]);
        return 2;
    }

    const char* socket_path = argv[1];

    int s;

    if ((s = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        return 1;
    }

    fprintf(stderr, "connecting...\n");

    struct sockaddr_un remote;

    remote.sun_family = AF_UNIX;
    if (strlen(socket_path) >= sizeof(remote.sun_path)) {
        fprintf(stderr, "path '%s' is too long for UNIX domain socket\n", socket_path);
        return 1;
    }
    strcpy(remote.sun_path, socket_path);
    socklen_t remote_len = sizeof(remote);
    // len = strlen(remote.sun_path) + sizeof(remote.sun_family);
    if (connect(s, (struct sockaddr *)&remote, remote_len) == -1) {
        perror("connect");
        return 1;
    }

    fprintf(stderr, "connection established\n");

    uint32_t left = 0;
    uint32_t right = 1000000000;
    uint32_t middle = 500000000;

    int attempt = 0;
    while (left <= right) 
    {
        uint32_t middle_nl = htonl(middle);
        if (!SendAll(s, (char*)&middle_nl, sizeof(uint32_t))) {
            fprintf(stderr, "sending request failed:(\nexiting...\n");
            return 1;
        }

        char result;
        if (!RecvAll(s, &result, sizeof(result))) {
            fprintf(stderr, "receiving response failed:(\nexiting...\n");
            return 1;
        }

        fprintf(stderr, "* attempt #%d: x %c %"PRIu32"\n", ++attempt, result, middle);

        if (result == '<') {
            right = middle - 1;
        } else if (result == '>') {
            left = middle + 1;
        } else {
            break;
        }
        middle = (left + right) / 2;
    }
    printf("%d\n", middle);

    close(s);

    return 0;
}
