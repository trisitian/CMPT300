#include <stdio.h>
#include <string.h>

int main(){
    char input[255]; // for the user to input
    char *arguments[15]; // array for arguments
    const char delimeter[2] = " ";
    while(strcmp(input, "exit") != 10){ // For some reason strcmp is returning 10 if they are equal not 0???? 
        int counter = 0; // for indexing array
        fgets(input, sizeof(input), stdin);
        arguments[counter] = strtok(input, delimeter); // get first argument
        while(arguments[counter] != NULL ){
            arguments[++counter] = strtok(NULL, delimeter); // get the rest of the arguments
        }
        
    }
    printf("goodbye");
    return 0;
}