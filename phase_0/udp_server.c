#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <pthread.h>

#define PORT 8080
#define BUFF_SIZE 10000

typedef struct{
    char message[BUFF_SIZE];
    struct sockaddr_in client_addr;
    int sockfd;
    socklen_t addr_len;
} client_data_t;

void string_reverse(char *str) {
    for (int start=0, end=strlen(str)-2; start<end; start++, end--) {
        char temp = str[start];
        str[start] = str[end];
        str[end] = temp; 
    }
}

void* handle_client(void* arg){
    client_data_t* data = (client_data_t*)arg;
    printf("[CLIENT MESSAGE] %s", data->message);
    string_reverse(data->message);

    sendto(data->sockfd, data->message, strlen(data->message), 0, (struct sockaddr*)&(data->client_addr), data->addr_len);

    free(data);
    pthread_exit(NULL);
}

int main(){
    char buffer[BUFF_SIZE];
    pthread_t thread_id;
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    socklen_t len;
    
    struct sockaddr_in serv_addr, client_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(PORT);

    bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));

    printf("[INFO] Server ready on port %d\n", PORT);

    while(1){
        len = sizeof(client_addr);
        ssize_t n = recvfrom(sockfd, buffer, BUFF_SIZE, 0,(struct sockaddr*)&client_addr, &len);
        buffer[n] = '\0';

        client_data_t* data = (client_data_t*)malloc(sizeof(client_data_t));
        strcpy(data->message, buffer);
        data->client_addr = client_addr;
        data->sockfd = sockfd;
        data->addr_len = len;

        if (pthread_create(&thread_id, NULL, handle_client, (void*)data) != 0){
            perror("Failed to create thread");
            free(data);
        }
        else pthread_detach(thread_id);
    }

    close(sockfd);
}