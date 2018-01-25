#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <libgen.h>

#include "lab2.h"

/*
 * client [server-ip] [port] [file]
 */
int main(int argc, char *argv[]) {
    if (argc < 4) {
        printf("Error\n");
        exit(1); 
    }

    int status;
    struct addrinfo hints;
    struct addrinfo *servinfo;
    int sockfd, numbytes;
    char buf[CHUNK];
    FILE *fp;
    int response_size;

    memset(&hints, 0, sizeof hints);

    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    // get ready to connect
    if ((status = getaddrinfo(argv[1], argv[2], &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
        exit(1);
    }

    // make a socket
    sockfd = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);

    // connect
    connect(sockfd, servinfo->ai_addr, servinfo->ai_addrlen);

    freeaddrinfo(servinfo);

    // client job
    fp = fopen(argv[3], "r");
    if (NULL == fp) {
        printf("Cannot open %s\n", argv[3]);
        exit(1);
    }
    // send size of the file (4 bytes)
    send_size(sockfd, get_file_size(fp));
    printf("Sent file size: %d\n", get_file_size(fp));

    // send file name (20 bytes)
    send_filename(sockfd, argv[3]);

    // send data
    send_file(sockfd, fp);

    // get response
    response_size = recv_size(sockfd);
    printf("Response get: server get %d bytes\n", response_size);

    // cleanup
    fclose(fp);
    close(sockfd);
    return 0;
}

