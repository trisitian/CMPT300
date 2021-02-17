#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>

typedef struct{
    char *name;
    char *value;
}EnvVar;

typedef struct{
        char *name;
        struct tm time;
    int code;
}Command;



int main(){
    char input[255]; // for the user to input
    char *arguments[15]; // array for arguments
    const char delimeter[2] = " ";
    char command[255];
    EnvVar EnvVariables[255]; // enviroment variables array, currently set to 255
    int enviromentCounter = 0; // currently setting a global counter, we could readjust so that it initializes with NULL values and find the first null value to edit



    while(strcmp(input, "exit") != 10){ // For some reason strcmp is returning 10 if they are equal not 0???? 
        int counter = 0; // for indexing array
        fgets(input, sizeof(input), stdin);
        arguments[counter] = strtok(input, delimeter); // get first argument
        while(arguments[counter] != NULL ){
            arguments[++counter] = strtok(NULL, delimeter); // get the rest of the arguments
        }
        /**check arguments[0]:
         * If is starts with $ create a new enviroment variable
         * If its log print the Command array
         * If it is print take arguments[1] to arguments [n] and print each value
         * If it starts with theme take argument[1] and change the prompt colour
         * otherwise call fork() on argument[0] and pass arguments[1] through arguments[n]
         * */
        strcpy(command, arguments[0]); // get the first argument
        if(command[0] == '$'){
            char *name = strtok(command, "="); // parse the name and value, push into array EnvVariables
            memmove(name, name+1, strlen(name));
            char *value = strtok(NULL, "=");
            EnvVar temp;
            temp.name = name;
            temp.value = value;
            EnvVariables[enviromentCounter] = temp;
            enviromentCounter++;
        }else if(strcmp(&command[0], "print") == 10){ // again with 10 .... not sure whats going on
            printf("Working");
        }

        
    }
    printf("goodbye");
    return 0;
}