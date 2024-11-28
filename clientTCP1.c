#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/wait.h>
#include <pthread.h>
#include "config.h"

#define CLIENT_PORT 4010

int client_fd;

int main () {

    client_fd = socket(AF_INET, SOCK_STREAM, 0); perror("socket");
    struct sockaddr_in addr = {
        .sin_family = AF_INET,
        .sin_addr.s_addr = INADDR_ANY, 
        .sin_port = htons (CLIENT_PORT),
    };

    bind(client_fd, (struct sockaddr*) &addr, sizeof(addr));perror("bind");

    struct sockaddr_in serv_addr = { 
        .sin_addr.s_addr = inet_addr("127.0.0.1"),
        .sin_port = htons(SERVER_PORT),
        .sin_family = AF_INET,
    };

    connect(client_fd, (struct sockaddr*) &serv_addr, sizeof(serv_addr));perror("connect");
    
    char buf_reponse[BUFSIZ]; memset(buf_reponse, 0, BUFSIZ);
    recv(client_fd, buf_reponse, sizeof(buf_reponse) -1, 0); perror("receive");
    printf("%s\n", buf_reponse);

    char reponse [BUFSIZ]; memset(reponse, 0, BUFSIZ);
    // strcpy(reponse,"Bonjour! Je suis le client 1\n");
    fgets(reponse, BUFSIZ, stdin);
    int error = send(client_fd, reponse, strlen(reponse), 0); perror("send");
       if(error == -1) {
        close(client_fd); return EXIT_FAILURE;
    }
   

    close(client_fd);

}