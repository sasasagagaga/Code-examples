
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

enum
{
    MAX_CONNECTIONS = 5,
};

int
main(int argc, char *argv[])
{
    int port = strtol(argv[1], NULL, 10);

    int fd = socket(AF_INET, SOCK_STREAM, 0);
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &(int) { 1 }, sizeof(int));

    struct sockaddr_in a1;
    a1.sin_family = AF_INET;
    a1.sin_port = htons(port);
    a1.sin_addr.s_addr = INADDR_ANY;
    bind(fd, (struct sockaddr *) &a1, sizeof(a1));

    listen(fd, MAX_CONNECTIONS);

    int sum = 0;
    while (1) {
        struct sockaddr_in a2;
        socklen_t slen = sizeof(a2);

        int afd = accept(fd, (struct sockaddr *) &a2, &slen);

        int num;
        read(afd, &num, sizeof(num));
        num = ntohl(num);
        sum += num;

        close(afd);

        if (num == 0) {
            break;
        }
    }

    close(fd);

    printf("%d\n", sum);
}

