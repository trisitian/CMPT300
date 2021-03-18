#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include "./list.h"
List *senderList, *recieverList;


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
    int *sockfd;                    // socket file descriptor
    struct sockaddr_in *socketInOut;// socket struct
};


// Will cause seg fault if user enters incorrect IP
int IPtoInt(char* normalForm){
    long first, second, third, fourth;
    const char delimeter[2] = ".";
    first = pow(256,3)*atoi(strtok(normalForm, delimeter));
    second = pow(256,2)*atoi(strtok(NULL, delimeter));
    third = 256*atoi(strtok(NULL, delimeter));
    fourth = atoi(strtok(NULL, delimeter));
    return first + second + third+ fourth;

}

/**
 * Basic ceaser cypher 
 * does changes in place @returns nothing
 * encryption function
 * */
void Encrypt(char message[4000]){
    for(int i = 0; message[i] != '\0'; i++){
        message[i] = (message[i] + 26) %256;
    }
}

/**
 * Basic ceaser cypher 
 * does changes in place @returns nothing
 * decryption function
 * */
void Decrypt(char message[4000]){
    for(int i = 0; message[i]  != '\0'; i++){
        message[i] = (message[i] -26) %256;
    }
}

// Thread for receiving messages
void *receivingThread(struct threadArg *threadArgs){

    int sockfd;
    if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0){ // IPv4, UDP, default protocal
        perror("Failed to create socket");
        exit(EXIT_FAILURE);
    } 

    struct sockaddr_in receiver, source;

    bzero(&receiver, sizeof(receiver));
    receiver.sin_family = AF_INET;
    receiver.sin_addr.s_addr = htonl(2130706433); // htonl(ip); // uncooment functions to use actual IPs
    receiver.sin_port = htons(6000); // htons(port);

    // bind socket to struct
    if ( bind(sockfd, (const struct sockaddr*) &receiver, sizeof(receiver)) < 0 ){
        perror("Failed to bind socket");
        exit(EXIT_FAILURE);
    }

    unsigned int sourceLen = sizeof(struct sockaddr_in);
    char buffer[4000];
    int bufferlen;
    while (1){
        bufferlen = recvfrom(sockfd, buffer, 4000, 0, (struct sockaddr *) &source, &sourceLen); 
        if (bufferlen < 0){
            perror("Failed to receive message");
            exit(1);
        }

        printf("%s", buffer);
        close(sockfd);
        break;

    }
    // DEPRECATED
//     close(threadArgs->fds[0]);
//     char buffer[4000];
//     struct sockaddr_in addressSource;
//     int sourceLength = sizeof(addressSource);
//     while (1){
//         // receive messages
//         if ( recvfrom(*(threadArgs->sockfd), buffer, 4000, 0, (struct sockaddr*) &addressSource, (unsigned int *)&sourceLength) < 0){
//             perror("Error receiving message");
//             exit(EXIT_FAILURE);
//         };
//         write(threadArgs->fds[1], buffer, 4000);
//     }
// }
    return 0;
}

// Thread for sending messages
void *sendingThread(struct threadArg *threadArgs){

    struct sockaddr_in receiver;

    int sockfd;
    if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0){ // IPv4, UDP, default protocal
        perror("Failed to create socket");
        exit(EXIT_FAILURE);
    } 

    receiver.sin_family = AF_INET;
    receiver.sin_addr.s_addr = htonl(2130706433); // htonl(ip); // uncooment functions to use actual IPs
    receiver.sin_port = htons(6000); // htons(port);    

    int sourceLen = sizeof(struct sockaddr_in);
    char *buffer = "Hello from the other side~";
    int bufferlen;

    bufferlen = sendto(sockfd, buffer, 27, 0, (const struct sockaddr *) &receiver, sourceLen);
    if (bufferlen < 0){
            perror("Failed to send message");
            exit(1);
    }
    close(sockfd);
    // DEPRECATED
    // struct sockaddr_in addressTo;
    // int sourceLength = sizeof(addressTo);
    
    // while (1){
    //     // send messages
    //     if ( sendto(*(threadArgs->sockfd), "Hello!", sizeof("Hello!"), 0, (struct sockaddr*) &addressTo, sourceLength) < 0 ){
    //         perror("Error sending message");
    //         exit(EXIT_FAILURE);
    //     }
    // }
    return 0;
}

// thread for printing into terminal
void *screenOutThread(struct threadArg *threadArgs){
    // close(fds[1]);
    // char buffer[4000];
    // while (1){
    //     while (read(fds[0], buffer, 4000) == 1){
    //         printf("%s",buffer);
    //     }
    // }
    return 0;
}

int main(int argc, char* argv[]){
    // check if correct num of args are passed. If not, exit
    if(argc != 4){
        perror("Error: missing argument. Enter port number on running machine, IP, and port to listen on.\n");
        exit(EXIT_FAILURE);
    }

    senderList = List_create();
    recieverList = List_create();
    
    pthread_t keyboardIn, UDPOut, UDPIn, screenOut; // threads to handle keyboard, UDP, and output
    struct threadArg threadArguments;               // for passing variables into threads
    int remoteClientIP, localPort, remotePort;
    localPort = atoi(argv[1]);
    if(strcmp(argv[2], "localhost") == 0){
        remoteClientIP = 2130706433; // 127.0.0.1 decimal 
    }else{
        remoteClientIP = IPtoInt(argv[2]);
    }
    remotePort = atoi(argv[3]);

    //printf("%s \n", test);

    // // init sockets
    // int sockfd;
    // if ( (sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0){ // IPv4, UDP, default protocal
    //     perror("Failed to create socket");
    //     exit(EXIT_FAILURE);
    // } 

    // struct sockaddr_in addressOut;
    // addressOut.sin_family = AF_INET;
    // addressOut.sin_addr.s_addr = htonl(2130706433); // htonl(ip); // uncooment functions to use actual IPs
    // addressOut.sin_port = htons(6000); // htons(port);

    // // bind socket to struct
    // if ( bind(sockfd, (const struct sockaddr*) &addressOut, sizeof(addressOut)) < 0 ){
    //     perror("Failed to bind socket");
    //     exit(EXIT_FAILURE);
    // }

    // threadArguments.socketInOut = &addressOut;
    // threadArguments.sockfd = &sockfd;

    //initialize threads
    pthread_create(&UDPIn, NULL, (void *)receivingThread, &threadArguments);
    pthread_create(&UDPOut, NULL, (void *)sendingThread, &threadArguments);
    pthread_create(&screenOut, NULL, (void *)screenOutThread, &threadArguments);
    
    pthread_join(UDPIn, NULL);
    pthread_join(UDPOut, NULL);
    pthread_join(screenOut, NULL);

    
    return 0;
}