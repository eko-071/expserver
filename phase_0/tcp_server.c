#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define PORT 8080
#define BUFF_SIZE 10000
#define MAX_ACCEPT_BACKLOG 5

void string_reverse(char *str) {
    for (int start=0, end=strlen(str)-2; start<end; start++, end--) {
        char temp = str[start];
        str[start] = str[end];
        str[end] = temp; 
    }
}

int main() {
    // Creating a listening socket here
    int listen_sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    // Setting the socket option to reuse local IPs and ports
    int enable = 1;
    setsockopt(listen_sock_fd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int));
    
    // Assigning an IPv4 address and port to our socket
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(PORT);
    // Actually binding socket to port
    bind(listen_sock_fd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    listen(listen_sock_fd, MAX_ACCEPT_BACKLOG);
    printf("[INFO] Server listening on port %d\n", PORT);

    // Creating a socket for the client stuff
    struct sockaddr_in client_addr;
    socklen_t client_addr_len;
    while(1) {
        // Accepting client connections
        int conn_sock_fd = accept(listen_sock_fd, (struct sockaddr *)&client_addr, &client_addr_len);
        printf("[INFO] Client connected to server\n");

        while (1) {
            // Creating a buffer for the client message
            char buff[BUFF_SIZE];
            memset(buff, 0, BUFF_SIZE);
            // Reading the client message to buffer
            ssize_t read_n = recv(conn_sock_fd, buff, sizeof(buff), 0);
            // Error handling
            if (read_n < 0) {
                printf("[ERROR] recv() failed\n");
                close(conn_sock_fd);
                break;
            }
            if (read_n == 0) {
                printf("[ERROR] Client disconnected\n");
                break;
            }

            // Client-side stuff: sending the reversed string
            printf("[CLIENT MESSAGE] %s", buff);
            string_reverse(buff);
            send(conn_sock_fd, buff, read_n, 0);
        }
        close(conn_sock_fd);
    }
}