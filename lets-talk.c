#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <arpa/inet.h>
#include <semaphore.h>
#include "./list.h"
List *senderList, *receiverList;
pthread_mutex_t lock;   // soon deprecated
sem_t mutexIN, mutexOUT;
bool socketStatus = false;
bool exitBool = false;

// struct to carry variables into threads
struct threadArg{
    char *ip;
    long portIN;
    long portOUT;
};

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

bool isValidIp(char* ip){
    struct sockaddr_in sa;
    int result = inet_pton(AF_INET, ip, &(sa.sin_addr));
    return result != 0;
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
    sem_wait(&mutexOUT);
    char input[4000];
    do{
        fgets(input, sizeof(input), stdin);
        removeNewline(input);
        // if(strcmp(input, "!status") == 0){
        //     pthread_mutex_lock(&lock);
        //     socketStatus = true;
        //     pthread_mutex_unlock(&lock);
        // }
        if (strcmp(input, "!status") == 0){
            socketStatus = true;
        }
        else if (strcmp(input, "!exit") == 0){
            exitBool = true;
        }
        encrypt(input);
        pthread_mutex_lock(&lock);
        List_add(senderList, input);
        pthread_mutex_unlock(&lock);
        // if(socketStatus){
        //     printf("checking status\n");
        //     if(socketStatus){
        //         printf("OFFLINE\n");
        //         pthread_mutex_lock(&lock);
        //         socketStatus = false;
        //         pthread_mutex_unlock(&lock);
        //     }else{
        //         printf("ONLINE\n");
        //     }
        // }
        // sem_post(&mutexOUT);
    }while(strcmp(input, "!exit") != 0);
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
    receiver.sin_addr.s_addr = htonl(INADDR_ANY); // takes address in network byte order
    receiver.sin_port = htons((*info).portIN); // htons(port);

    // bind socket to socket address
    if ( bind(sockfd, (const struct sockaddr*) &receiver, sizeof(receiver)) < 0 ){
        perror("Failed to bind socket");
        exit(EXIT_FAILURE);
    }

    unsigned int sourceLen = sizeof(source);
    char buffer[4000];
    int bufferlen;
    // sem_post(&mutexIN);
    // should be able to do without the while loop, as recvfrom seems to hang the thread until message is received
    while (1){
        // recvfrom will wait for a message, so no need for a lock
        bufferlen = recvfrom(sockfd, buffer, 4000, 0, (struct sockaddr *) &source, &sourceLen);
        fflush(stdout);
        if (bufferlen < 0){
            perror("Failed to receive message");
            exit(EXIT_FAILURE);
        }
        pthread_mutex_lock(&lock);
        List_add(receiverList, buffer);
        pthread_mutex_unlock(&lock);
        // sem_post(&mutexIN);
        // sem_wait(&mutexIN);
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
    int sourceLen = sizeof(receiver);
    receiver.sin_family = AF_INET;
    if(isValidIp((*info).ip)){
        receiver.sin_addr.s_addr = inet_addr((*info).ip);
    }else{
        printf("Invalid IP entered, setting socket to localhost\n");
        receiver.sin_addr.s_addr = inet_addr("127.0.0.1");
    }
    receiver.sin_port = htons((*info).portOUT); // htons(port); 
    char *buffer;
    int bufferlen;
    sem_post(&mutexOUT);
    while(1){
        // send message
        while(List_count(senderList) != 0){
            buffer = List_curr(senderList);
            //decrypt(buffer);
            
            bufferlen = sendto(sockfd, buffer, 27, 0, (const struct sockaddr *) &receiver, sourceLen);
            pthread_mutex_lock(&lock);
            List_remove(senderList);
            pthread_mutex_unlock(&lock);
            if (bufferlen < 0){
                perror("Failed to send message");
                exit(EXIT_FAILURE);
            }
            if (exitBool){
                exit(0);
            }
        }
        // sem_wait(&mutexOUT);
    }
    close(sockfd);
    return 0;
}

// thread for printing into terminal
void *screenOutThread(struct threadArg *threadArgs){
    // sem_wait(&mutexIN);
    char *buffer;
    char onlineYes[7] = "Online";
    char onlineNo[8] = "Offline";
    encrypt(onlineYes);
    while(1){
        if(List_count(receiverList) != 0){
            buffer = List_first(receiverList);
            if (strcmp(buffer, "Offline") != 0){
                decrypt(buffer);
            }
            if (strcmp(buffer, "!exit") == 0){
                exit(0);
            }
            else if(strcmp(buffer, "!status") == 0){
                // if an incoming message is for checking status
                // shoot back a message saying Online
                // skip printing the message
                List_add(senderList, onlineYes);
                List_remove(receiverList);
            }
            else{
                printf("%s\n", buffer);
                fflush(stdout);
                pthread_mutex_lock(&lock);
                List_remove(receiverList);
                pthread_mutex_unlock(&lock);
            }
        }

        // if !status is sent
        if (socketStatus){
            sleep(1);
            if(List_count(receiverList) == 0){
                List_add(receiverList, onlineNo);
            }
            socketStatus = false;
        }
        // sem_wait(&mutexIN);
    }
    return 0;
}

int main(int argc, char* argv[]){
    sem_init(&mutexIN, 0, 0); // initialize semaphore for receiveThread & screenOut
    sem_init(&mutexOUT, 0, 0); // initialize semaphore for awaitInput & sendingThread
    
    // check if correct num of args are passed. If not, exit
    if(argc != 4){
        perror("Error: missing argument. Enter port number on running machine, IP, and port to listen on.\n");
        exit(EXIT_FAILURE);
    }

    // create lists for conversations
    senderList = List_create();
    receiverList = List_create();

    pthread_t keyboardIn, UDPOut, UDPIn, screenOut; // threads to handle keyboard, UDP, and output

    // for passing variables into threads
    struct threadArg threadArguments;                   
    threadArguments.portIN = atoi(argv[1]);         // assign incoming port (my port)
    threadArguments.portOUT = atoi(argv[3]);        // assign outgoing port (remote port)
    
    if(strcmp(argv[2], "localhost") == 0){
        threadArguments.ip = "127.0.0.1"; // localhost 
    }else{
        threadArguments.ip = argv[2];
    }

    //initialize threads
    pthread_create(&keyboardIn, NULL, awaitInput, NULL);
    pthread_create(&UDPIn, NULL, (void *)receivingThread, &threadArguments);
    pthread_create(&UDPOut, NULL, (void *)sendingThread, &threadArguments);
    pthread_create(&screenOut, NULL, (void *)screenOutThread, NULL);
    
    pthread_join(keyboardIn, NULL);
    exit(0);
    pthread_join(UDPOut, NULL);
    exit(0);
    pthread_join(UDPIn, NULL);
    exit(0);
    pthread_join(screenOut, NULL);
    exit(0);
    // pthread_mutex_destroy(&lock);
    // return 0;
}