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
sem_t mutexIN, mutexOUT, mutexKeyboard;
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

/**
 * utility function for setting up socket with proper IP
 * checks validity of ip adress
 * */
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
    char *input;   // max input limit 4000 characters
    char *secondary;
    size_t size = 4000;
    input = (char* ) malloc(size);
    secondary = (char*) malloc(size); 
    printf("Welcome to Lets-Talk! Please type your messages now.\n");
    do{
        //TODO: make compatible with pasting multiple lines into terminal
        //ALL 3 OF THEESE SOLUTIONS SHOULD WORK, FOR SOME REASON THE LOOP NEVER EXITS, DOES IT HAVE SOMETHING TO DO WITH LOCKS?
        //while(1){
            //fgets(input, sizeof(input), stdin);
            // if(strcmp(secondary, "\n")){
            //     break;
            // }
            //strcat(input, secondary); 
        //}
        // while(!feof(stdin) || (fgets(secondary, sizeof(secondary), stdin) != NULL)){
        //     if(strcmp(fgets(secondary, sizeof(secondary), stdin),"\n") == 0){
        //         break;
        //     }
        //     fgets(secondary, sizeof(secondary), stdin);
        //     strcat(input, secondary);
        // }
        // for(int i = 0; i < 100; i++){
        //     if(fgets(secondary, sizeof(secondary), stdin) == NULL){
        //         break;
        //     }
        //     fgets(secondary, sizeof(secondary), stdin);
        //     strcat(input, secondary); 
        // }
        //fgets(input, sizeof(input), stdin);
        // int prev, curr;
        // while(1){
        //     getline(&secondary, &size, stdin);
        //     curr = sizeof(secondary);
        //     if(curr == prev){
        //         break;
        //     } else{
        //         printf("SIZE IS %ld\n", sizeof(secondary));
        //     }
        //     prev = sizeof(secondary);
        //     strcat(input, secondary);
        // }
        getline(&input, &size,stdin);

        removeNewline(input);

        // if asking for status, change socketStatus to true for screenoutThread
        // activate screenoutThread
        if (strcmp(input, "!status") == 0){
            pthread_mutex_lock(&lock);
            socketStatus = true;
            pthread_mutex_unlock(&lock);
            sem_post(&mutexIN);
        }
        // if asking to exit, change exitBool to true to signal threads to terminate
        else if (strcmp(input, "!exit") == 0){
            exitBool = true;
        }
        // encrypt and push into senderList
        encrypt(input);
        pthread_mutex_lock(&lock);
        List_add(senderList, input);
        pthread_mutex_unlock(&lock);
        
        // activate sendingThread
        sem_post(&mutexOUT);
        sem_wait(&mutexKeyboard);

    }while(strcmp(input, "!exit") != 0);    // if !exit entered, terminate thread
    return 0;
}

// Thread for receiving messages
void *receivingThread(void *threadArguments){
    // make threadArguments local
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
    receiver.sin_addr.s_addr = htonl(INADDR_ANY); 
    receiver.sin_port = htons((*info).portIN); 

    // bind socket to socket address
    if ( bind(sockfd, (const struct sockaddr*) &receiver, sizeof(receiver)) < 0 ){
        perror("Failed to bind socket");
        exit(EXIT_FAILURE);
    }

    unsigned int sourceLen = sizeof(source);
    char buffer[4000];
    int bufferlen;
    
    while (1){
        // recvfrom will wait for a message, so no need for a lock
        bufferlen = recvfrom(sockfd, buffer, 4000, 0, (struct sockaddr *) &source, &sourceLen);
        fflush(stdout);
        if (bufferlen < 0){
            perror("Failed to receive message");
            exit(EXIT_FAILURE);
        }
        
        // add received message to receiverList, activate screenOutThread
        pthread_mutex_lock(&lock);
        List_add(receiverList, buffer);
        pthread_mutex_unlock(&lock);
        sem_post(&mutexIN);
    }

    close(sockfd);
    return 0;
}

// Thread for sending messages
void *sendingThread(void *threadArguments){
    // make threadArguments local
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
    // check if IP is entered correctly
    if(isValidIp((*info).ip)){
        receiver.sin_addr.s_addr = inet_addr((*info).ip);
    }else{
        printf("Invalid IP entered, setting socket to localhost\n");
        receiver.sin_addr.s_addr = inet_addr("127.0.0.1");
    }
    receiver.sin_port = htons((*info).portOUT);
    
    char *buffer;
    int bufferlen;
    
    while(1){
        while(List_count(senderList) != 0){
            buffer = List_curr(senderList); // grab latest item in senderList         
            bufferlen = sendto(sockfd, buffer, 4000, 0, (const struct sockaddr *) &receiver, sourceLen);
            // remove the sent item from list
            pthread_mutex_lock(&lock);
            List_remove(senderList);
            pthread_mutex_unlock(&lock);

            if (bufferlen < 0){
                perror("Failed to send message");
                exit(EXIT_FAILURE);
            }
            if (exitBool){  // terminate thread if exit was entered in awaitInput
                exit(0);
            }
        }

        // waits for input from awaitInput,
        // or if screenOutThread sees a !status and wants to send a reply.
        sem_post(&mutexKeyboard);
        sem_wait(&mutexOUT);
    }

    close(sockfd);
    return 0;
}

// thread for printing into terminal
void *screenOutThread(struct threadArg *threadArgs){
    char *buffer;
    // onlineYes is used to send over to the other client via UDP,
    // onlineNo is used to tell this client that the remote client isn't online.
    char onlineYes[7] = "Online";
    char onlineNo[8] = "Offline";
    encrypt(onlineYes); // for sending over UDP
    while(1){
        // unlocks when awaitInput grabs a !status string,
        // or when receivingThread receives a message.
        sem_wait(&mutexIN);
        
        // if awaitInput grabs a !status string
        if (socketStatus){
            sleep(1);   // wait while remote machine replies
            // if nothing is received from the remote client,
            // (also means receivingThread is still stuck at recvfrom),
            // append the onlineNo message as if "Offline" was received.
            if(List_count(receiverList) == 0){
                List_add(receiverList, onlineNo);
            }
            // if a reply is received, 
            // call sem_wait to cancel out the sem_post called by the receivingThread.
            else{
                sem_wait(&mutexIN);
            }
            pthread_mutex_lock(&lock);
            socketStatus = false;
            pthread_mutex_unlock(&lock);
        }

        buffer = List_first(receiverList);
        // onlineNo is stored as plaintext, so no need to decrypt.
        // if onlineNo was a encrypted text and the check is removed,
        // it will decrypt the string every time it's called,
        // eventually turning it into some other string.
        if (strcmp(buffer, "Offline") != 0){
            decrypt(buffer);
        }
        // terminate this client if !exit was called by the remote client
        if (strcmp(buffer, "!exit") == 0){
            exit(0);
        }
        // if remote client is asking for this client's status...
        else if(strcmp(buffer, "!status") == 0){
            // shoot back a message saying Online;
            // skip printing this message.
            List_add(senderList, onlineYes);
            sem_post(&mutexOUT);
            List_remove(receiverList);
            continue;
        }
        printf("%s\n", buffer);
        fflush(stdout);
        // remove the printed item from the list
        pthread_mutex_lock(&lock);
        List_remove(receiverList);
        pthread_mutex_unlock(&lock);
    }
    return 0;
}

int main(int argc, char* argv[]){
    sem_init(&mutexIN, 0, 0); // initialize semaphore for receiveThread & screenOut
    sem_init(&mutexOUT, 0, 0); // initialize semaphore for awaitInput & sendingThread
    sem_init(&mutexKeyboard, 0, 0); // initialize semaphore for awaitInput & sendingThread
    
    // check if correct num of args are passed. If not, exit.
    if(argc != 4){
        perror("Error: missing argument. Enter port number on running machine, IP, and port to listen on.\n");
        exit(EXIT_FAILURE);
    }

    // create lists for conversations
    senderList = List_create();
    receiverList = List_create();

    // threads to handle keyboard, UDP, and output
    pthread_t keyboardIn, UDPOut, UDPIn, screenOut; 

    // for passing variables into threads
    struct threadArg threadArguments;                   
    threadArguments.portIN = atoi(argv[1]);         // assign incoming port (my port)
    threadArguments.portOUT = atoi(argv[3]);        // assign outgoing port (remote port)
    if(strcmp(argv[2], "localhost") == 0){          // assign IP
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
    pthread_join(UDPOut, NULL);
    pthread_join(UDPIn, NULL);
    pthread_join(screenOut, NULL);
    pthread_mutex_destroy(&lock);
    return 0;
}