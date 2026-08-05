#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define SERVER_PORT 8000
#define SERVER_ADDR "127.0.0.1"
#define BUFF_SIZE 10000

int main() {
    // Creating a listening socket
    int client_sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (client_sock_fd < 0) {
        perror("Socket creation error");
        exit(1);
    }

    // Setting up the server address
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(SERVER_ADDR);
    server_addr.sin_port = htons(SERVER_PORT);

    // Connecting to the TCP server
    if (connect(client_sock_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) != 0) {
        printf("[ERROR] Failed to connect to TCP server\n");
        exit(1);
    }
    else {
        printf("[INFO] Connected to TCP server\n");
    }

    char *line;
    size_t line_len = 0, read_n;

    while (1) {
        // Get the message
        read_n = getline(&line, &line_len, stdin);
        
        // Sending message to TCP server
        send(client_sock_fd, line, read_n, 0);
        printf("[CLIENT] Message sent\n");

        char buff[BUFF_SIZE];
        memset(buff, 0, sizeof(buff));
        read_n = recv(client_sock_fd, buff, sizeof(buff), 0);

        // Error handling
        if (read_n <= 0) {
            printf("[CLIENT] Server disconnected\n");
            close(client_sock_fd);
            exit(1);
        }

        printf("[SERVER MESSAGE] %s", buff);
    }

    return 0;
}