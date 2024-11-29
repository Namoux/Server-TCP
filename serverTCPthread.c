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

int nb_clients = 0;
int server_fd;
int error;

void* clrecvsend (void* arg) {
    long int client_fd = (long int) arg;
    
    while(1) {
        // 5. send
        char response[BUFSIZ] = {"Bienvenue ! Je suis le serveur!\n"};
        error = send(client_fd, response, strlen(response), 0); perror("send ");
        if(error == -1) return NULL;
        /** 6.  recv
         * Lecture du file descriptor en boucle pour recevoir les requete client
         */
        char reponse [BUFSIZ]; memset(reponse, 0, BUFSIZ);
        int reponse_length = recv(client_fd, reponse, BUFSIZ, 0); perror("recv ");
        if(reponse_length <=0 ) { // Si le recv échoue !
            // Close the connexion to the client
            close(client_fd); perror("close");
            printf("Client disconnected !\n");
            return NULL;
        }   

        printf ("$%s\n", reponse);
    }
}

void debug_sleep() {  // Debug sleep pour voir les étapes défiler lentement.
    printf("\n");
    sleep(1);
    fflush(stdout);
}

int main () {

    /** 1. CREATION DU SOCKET
    * Je crée un socket avec la fonction socket()
    * Je lui fournit : 
    * - Sa famille de socket (AF_INET pour les socket réseau)
    * - Une IP (INADDR_ANY pour toutes les ip possible : 127.0.0.1 ou autres)
    * - Un port
    */
    server_fd = socket(AF_INET, SOCK_STREAM, 0); perror("socket ");
    if(server_fd == -1) return EXIT_FAILURE; // Si le serveur ne se lance, quitte le programme

    struct sockaddr_in server_addr = {
        .sin_family = AF_INET,
        .sin_addr.s_addr = INADDR_ANY,
        .sin_port = htons(SERVER_PORT) // convert int port to binary port
    };

    // debug_sleep();

    /** 2. Bind(liaison) du socket à un file_descriptor
     */
    error = bind(server_fd,(struct sockaddr*)&server_addr, sizeof(server_addr)); perror("bind ");
    if(error == -1) return EXIT_FAILURE;

    // debug_sleep(); 

    /** 3.  Listen
     * Lancer l'écoute du serveur
     */
    error = listen(server_fd, BUFSIZ); perror("listen ");
    if(error == -1) return EXIT_FAILURE;
    printf("Server listen to 127.0.0.1:%d\n", SERVER_PORT);

    // debug_sleep();

    pthread_t thread;
    struct sockaddr_in client_addr;
    socklen_t len;
    long int client_fd;

    while(1) {
        client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &len);
        perror ("accept ");
        nb_clients++;   
        printf("accept numero : %d\n", nb_clients);
        printf("Wait for a client to connect...\n");   
        pthread_create(&thread, NULL, clrecvsend, (void*)client_fd); perror("pthread_create ");
        printf("Thread starts...\n");
    }
return EXIT_SUCCESS;
}