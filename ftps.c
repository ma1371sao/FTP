#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "lab2.h"

/*
 * server [port]
 */
int main(int argc, char *argv[]) {
    // check command line arguments
    if (argc != 2) {
        printf("Invalid number of command line arguments\n");
        exit(1);
    }

    int status;
    struct addrinfo hints;
    struct addrinfo *servinfo;
    int sockfd, new_fd;
    struct sockaddr_storage their_addr;
    socklen_t addr_size;

    memset(&hints, 0, sizeof hints);

    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    if ((status = getaddrinfo(NULL, argv[1], &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
        exit(1);
    }

    // make a socket
    sockfd = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);
    if (sockfd == -1) {
        perror("server: socket");
        exit(1);
    }
    // bind to the port
    if ((status = bind(sockfd, servinfo->ai_addr, servinfo->ai_addrlen)) == -1) {
        close(sockfd);
        perror("server: bind");
        exit(1);
    }

    freeaddrinfo(servinfo);

    // listen
    if ((status = listen(sockfd, BACKLOG)) == -1) {
        perror("listen");
        exit(1);
    }

    printf("server: waiting for connections...\n");

    while (1) {
        // new accept an incoming connection
        addr_size = sizeof their_addr;
        new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &addr_size);
        if (new_fd == -1) {
            perror("accept");
            continue;
        }

        if (!fork()) { // child process
            close(sockfd);

            // recv file size 
            int size = recv_size(new_fd);
            printf("Received file size: %d\n", size);

            // recv file name
            char* filename = malloc(FILENAME_CHUNK * sizeof(char));
            recv_filename(new_fd, filename);
            printf("Filename: %s\n", filename);

            // construct path
            mkdir("./recvd", 0777);
            char* path = malloc((6 + strlen(filename)) * sizeof(char));
            strcpy(path, "recvd/"); 
            strcat(path, filename);
            printf("path: %s\n", path);
            FILE* fp = fopen(path, "wb");
            if (NULL == fp) {
                printf("Cannot open %s\n", path);
            }

            // recv data
            int numbytes = recv_file(new_fd, fp, size, filename);

            // send response
            send_size(new_fd, numbytes);
            printf("Sent response: received %d bytes of data\n", numbytes);

            // clean up
            fclose(fp);
            free(filename);
            free(path);
            close(new_fd);
            exit(0);
        }
        close(new_fd);
    }

    return 0;
}

