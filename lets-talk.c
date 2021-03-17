#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <unistd.h>
#include "./list.h"
List *messageList;

// not quite working, cipher should go something like this
// char* Encrypt(char* message){
//     char encryptedMessage[256];
//     char temp;
//     for(int i = 0; i < strlen(message); i++){
//         temp = message[i] + 26;
//         strncat(encryptedMessage, &temp, 1);
//     }
//     return encryptedMessage;
// }

// char* Decrypt(char* message){
//     char decryptedMessage[256];
//     char temp;
//     for(int i = 0; i < strlen(message); i++){
//         temp = message[i] - 26;
//         decryptedMessage[i] = temp;
//     }
//     return decryptedMessage;
// }


//FUNCTION FOR KEYBOARD INPUT
// void *awaitInput(void *ptr){
//     char line;
//     scanf("%c", &line);
//     while(line != "!exit"){
//         scanf("%c", line);
//     }
// }

// struct to carry variables into threads
struct threadArg{
    int fds[2];                     // pipe
    int *sockfd;                    // socket file descriptor
    struct sockaddr_in *socketInOut;// socket struct
};

// Thread for receiving messages
void *receivingThread(struct threadArg *threadArgs){
    close(threadArgs->fds[0]);
    char buffer[4000];
    struct sockaddr_in addressSource;
    int sourceLength = sizeof(addressSource);
    while (true){
        // receive messages
        if ( recvfrom(*(threadArgs->sockfd), buffer, 4000, 0, (struct sockaddr*) &addressSource, (unsigned int *)&sourceLength) < 0){
            perror("Error receiving message");
            exit(EXIT_FAILURE);
        };
        write(threadArgs->fds[1], buffer, 4000);
    }
}

// Thread for sending messages
void *sendingThread(struct threadArg *threadArgs){
    struct sockaddr_in addressTo;
    int sourceLength = sizeof(addressTo);
    
    while (true){
        // send messages
        if ( sendto(*(threadArgs->sockfd), "Hello!", sizeof("Hello!"), 0, (struct sockaddr*) &addressTo, sourceLength) < 0 ){
            perror("Error sending message");
            exit(EXIT_FAILURE);
        }
    }
}

// thread for printing into terminal
void *screenOutThread(int fds[2]){
    close(fds[1]);
    char buffer[4000];
    while (true){
        while (read(fds[0], buffer, 4000) == 1){
            printf("%s",buffer);
        }
    }
}

int main(int argc, char* argv[]){
    // check if correct num of args are passed. If not, exit
    if(argc != 5){
        perror("Error: missing argument. Enter port number on running machine, IP, and port to listen on.\n") ;
        exit(EXIT_FAILURE);
    }

    messageList = List_create();
    
    pthread_t keyboardIn, UDPOut, UDPIn, screenOut; // threads to handle keyboard, UDP, and output
    struct threadArg threadArguments;               // for passing variables into threads
    const int remoteClientIP, localPort, remotePort;// TODO: make pointers from struct to constant variables

    // init pipe
    if (pipe(threadArguments.fds) < 0){
        perror("Failed to create pipe.");
        exit(EXIT_FAILURE);
    }

    // init sockets
    int sockfd;
    if ( (sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0){ // IPv4, UDP, default protocal
        perror("Failed to create socket");
        exit(EXIT_FAILURE);
    } 

    struct sockaddr_in addressOut;
    addressOut.sin_family = AF_INET;
    addressOut.sin_addr.s_addr = INADDR_ANY; // htonl(ip); // uncooment functions to use actual IPs
    addressOut.sin_port = htons(6000); // htons(port);

    // bind socket to struct
    if ( bind(sockfd, (const struct sockaddr*) &addressOut, sizeof(addressOut)) < 0 ){
        perror("Failed to bind socket");
        exit(EXIT_FAILURE);
    }

    threadArguments.socketInOut = &addressOut;
    threadArguments.sockfd = &sockfd;

    // initialize threads
    pthread_create(&UDPIn, NULL, (void *)receivingThread, &threadArguments);
    pthread_create(&UDPOut, NULL, (void *)sendingThread, &threadArguments);
    pthread_create(&screenOut, NULL, (void *)screenOutThread, threadArguments.fds);
    
    // char *test;
    // test = Encrypt("hello world");
    // printf("%s \n", test);
    // printf("%d", List_count(messageList));
    return 0;
}