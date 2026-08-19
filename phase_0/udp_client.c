#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define SERVER_PORT 8080
#define SERVER_ADDR "127.0.0.1"
#define BUFF_SIZE 10000

int main(){
    char buffer[BUFF_SIZE];
    char message[BUFF_SIZE];

    int client_sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
    
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(SERVER_ADDR);
    server_addr.sin_port = htons(SERVER_PORT);

    while (1){
        printf("Enter a string: ");
        fgets(message, BUFF_SIZE, stdin);

        // Sending the message to server
        sendto(client_sock_fd, message, strlen(message), 0, (struct sockaddr*)&server_addr, sizeof(server_addr));

        // Receiving the message from server now
        ssize_t n = recvfrom(client_sock_fd, buffer, BUFF_SIZE, 0, NULL, NULL);
        buffer[n] = '\0';

        printf("[SERVER MESSAGE] %s", buffer);
    }

    // Closing the socket
    close(client_sock_fd);
    return 0;
}