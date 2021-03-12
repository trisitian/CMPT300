#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include "./list.h"
List *messageList = List_create();
//not quite working, cipher should go something like this
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
void *awaitInput(void *ptr){
    char line;
    scanf("%c", &line);
    while(line != "!exit"){
        scanf("%c", line);
    }
}



int main(int argc, char* argv[]){

    // see in instructions, 4 threads as asked to create
    pthread_t keyboardIn, UDPOut, UDPIn, screenOut; 
    //see instrunctions, 3 arguments that are to be passed in when lets-talk is called
    int remoteClientIP, localPort, remotePort;
    int length = 0;
    while(argv[++length] != NULL);// did you know that there is no fricken "size of" for arrays in c? and the method you do use doesn't seem to work!??!?! ugh I miss c++ 
    if(length != 4){
        printf("Error missing argument, enter port number on running machine, IP, and port to listen on\n") ;
        return 1;
    }
    // as per convention in her examples
    localPort = (int)argv[1];
    remoteClientIP = (int)argv[2];
    remotePort = (int)argv[3];
    // char *test;
    // test = Encrypt("hello world");
    // printf("%s \n", test);

    return 0;
}