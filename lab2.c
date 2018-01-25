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

int get_file_size(FILE *fp) {
    int size;

    fseek(fp, 0, SEEK_END);
    size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    return size;
}

void send_size(int sockfd, int size) {
    uint32_t conv = htonl(size);
    write(sockfd, &conv, sizeof(conv));
}

void send_filename(int sockfd, char* path) {
    char* filename;
    filename = basename(path);

    char buf[FILENAME_CHUNK];
    if (strlen(filename) < FILENAME_CHUNK) {
        strcpy(buf, filename);
    } else {
        memcpy(buf, &filename, FILENAME_CHUNK);
    }

    int rc = write(sockfd, filename, FILENAME_CHUNK);
    printf("Sent filename: %s, number of bytes: %d\n", filename, rc);
}

void send_file(int sockfd, FILE *fp) {
    char buf[CHUNK];
    size_t numbytes;
    unsigned int outbytes;

    while (!feof(fp)) {
        if ((numbytes = fread(buf, sizeof(buf[0]), CHUNK, fp)) > 0) {
            //printf("read bytes: %d\n", numbytes);
            outbytes = write(sockfd, buf, numbytes);
            //printf("send out bytes: %d\n", outbytes);
        }
    }
}

int recv_size(int new_fd) {
   int num, size; 

   read(new_fd, &num, sizeof(num));
   size = ntohl(num);

   return size;
}

int recv_filename(int new_fd, char* name) {
    int res;
    char filename[FILENAME_CHUNK];

    res = read(new_fd, filename, FILENAME_CHUNK);
    printf("recv file name: %s\n", filename);
    
    strcpy(name, filename);

    return 0;
}

int recv_file(int new_fd, FILE* fp, int size, char* filename) {
    int res;
    char buf[CHUNK];
    int size_get = 0;

    while(size_get < size) {
        res = read(new_fd, buf, CHUNK);
        if (res == 0) {
            break;
        }
        fwrite(buf, sizeof(buf[0]), res, fp);
        size_get += res;
        //printf("Recv file size: %d\n", res);
    }

    printf("file '%s' transfer done\n", filename);
    return size_get;
}

