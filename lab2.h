#define CHUNK 1000
#define SIZE_CHUNK 4
#define FILENAME_CHUNK 20
#define BACKLOG 5

int recv_size(int new_fd);

int recv_filename(int new_fd, char* name);

int recv_file(int new_fd, FILE* fp, int size, char* filename);

void send_size(int sockfd, int size);

int get_file_size(FILE *fp);

void send_filename(int sockfd, char* path);

void send_file(int sockfd, FILE *fp);

