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
// struct to carry variables into threads
struct threadArg{
    long ip;
    long portIN;
    long portOUT;
};

// Will cause seg fault if user enters incorrect IP
long IPtoInt(char* normalForm){
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
void encrypt(char message[4000]){
    for(int i = 0; message[i] != '\0'; i++){
        message[i] = (message[i] + 26) %256;
    }
}

/**
 * Basic ceaser cypher 
 * does changes in place @returns nothing
 * decryption function
 * */
void decrypt(char message[4000]){
    for(int i = 0; message[i]  != '\0'; i++){
        message[i] = (message[i] -26) %256;
    }
}

/**
 * remove new line, borrowed from assignment2
 * */
void removeNewline(char input[4000]) {
    for (int i = 0; i < 4000; i++) {
        if (input[i] == '\n') {
            input[i] = '\0'; 
            break;
        }
    }
}
//Thread for getting keyboard input
void *awaitInput(void *ptr){
    char input[4000];
    fgets(input, sizeof(input), stdin);
    removeNewline(input);
    while(strcmp(input, "!exit") !=0){
        encrypt(input);
        //LOCK WILL NEED TO GO HERE
        List_add(senderList, input);
        printf("There are %d arguments in the list\n", List_count(senderList));
        //LOCK WILL NEED TO UNLOCK HERE
        fgets(input, sizeof(input), stdin);
        removeNewline(input);
    }
    return 0;
}
// Thread for receiving messages
void *receivingThread(void *threadArguments){
    struct threadArg *info = (struct threadArg*)threadArguments;
    // create socket
    int sockfd;
    if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0){ // IPv4, UDP, default protocal
        perror("Failed to create socket");
        exit(EXIT_FAILURE);
    } 

    // assign values to socket address
    struct sockaddr_in receiver, source;
    bzero(&receiver, sizeof(receiver));
    receiver.sin_family = AF_INET;
    receiver.sin_addr.s_addr = htonl((*info).ip); // htonl(ip); // uncooment functions to use actual IPs
    receiver.sin_port = htons((*info).portIN); // htons(port);

    // bind socket to socket address
    if ( bind(sockfd, (const struct sockaddr*) &receiver, sizeof(receiver)) < 0 ){
        perror("Failed to bind socket");
        exit(EXIT_FAILURE);
    }

    unsigned int sourceLen = sizeof(struct sockaddr_in);
    char buffer[4000];
    int bufferlen;
    // should be able to do without the while loop, as recvfrom seems to hang the thread until message is received
    while (1){
        bufferlen = recvfrom(sockfd, buffer, 4000, 0, (struct sockaddr *) &source, &sourceLen); 
        if (bufferlen < 0){
            perror("Failed to receive message");
            exit(EXIT_FAILURE);
        }
        //add lock here
        List_add(recieverList, buffer);
        // add unlock here
    }
    close(sockfd);

    return 0;
}

// Thread for sending messages
void *sendingThread(void *threadArguments){
    struct threadArg *info = (struct threadArg*)threadArguments;

    // create socket
    int sockfd;
    if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0){ // IPv4, UDP, default protocal
        perror("Failed to create socket");
        exit(EXIT_FAILURE);
    } 

    // assign values to socket address
    struct sockaddr_in receiver;
    receiver.sin_family = AF_INET;
    receiver.sin_addr.s_addr = htonl((*info).ip); // htonl(ip); // uncooment functions to use actual IPs
    receiver.sin_port = htons((*info).portOUT); // htons(port);    

    int sourceLen = sizeof(struct sockaddr_in);
    char *buffer = "Hello from the other side~";
    int bufferlen;
    while(1){
    // send message
        while(List_count(senderList) != 0){
            //printf("There are currently %d messages in the list\n", List_count(senderList));
            buffer = List_curr(senderList);
            //decrypt(buffer);
            bufferlen = sendto(sockfd, buffer, 27, 0, (const struct sockaddr *) &receiver, sourceLen);
            List_remove(senderList);
            //printf("There are currently %d messages in the list\n", List_count(senderList));
            if (bufferlen < 0){
                perror("Failed to send message");
                exit(EXIT_FAILURE);
            }
        }
    }
    close(sockfd);
    return 0;
}

// thread for printing into terminal
void *screenOutThread(struct threadArg *threadArgs){
    char *buffer;
    while(1){
        if(List_count(recieverList) != 0){
            buffer = List_first(recieverList);
            decrypt(buffer);
            printf("%s\n", buffer);
            List_remove(recieverList);
        }
    }
    return 0;
}

int main(int argc, char* argv[]){
    // check if correct num of args are passed. If not, exit
    if(argc != 4){
        perror("Error: missing argument. Enter port number on running machine, IP, and port to listen on.\n");
        exit(EXIT_FAILURE);
    }

    // create lists for conversations
    senderList = List_create();
    recieverList = List_create();

    // for passing variables into threads
    struct threadArg threadArguments;               
    
    pthread_t keyboardIn, UDPOut, UDPIn, screenOut; // threads to handle keyboard, UDP, and output
    threadArguments.portIN = atoi(argv[1]);
    if(strcmp(argv[2], "localhost") == 0){
        threadArguments.ip = 2130706433; // 127.0.0.1 decimal 
    }else{
        threadArguments.ip = IPtoInt(argv[2]);
    }
    threadArguments.portOUT = atoi(argv[3]);

    //initialize threads
    pthread_create(&keyboardIn, NULL, awaitInput, NULL);
    pthread_create(&UDPOut, NULL, (void *)sendingThread, &threadArguments);
    pthread_create(&UDPIn, NULL, (void *)receivingThread, &threadArguments);
    pthread_create(&screenOut, NULL, (void *)screenOutThread, NULL);
    
    pthread_join(keyboardIn, NULL);
    pthread_join(UDPOut, NULL);
    pthread_join(UDPIn, NULL);
    pthread_join(screenOut, NULL);

    
    return 0;
}