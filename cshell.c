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

/*
prints the string with the proper colour formatting
colour can be redefined by user. Default is red.
*/
void shell_print(int colour, const char* STRING){
    printf("\033[0;%im%s\033[0;37m", colour, STRING);
}

int change_theme(const char* NEW_COLOUR, int old_colour){
    if (strcmp(NEW_COLOUR, "red") == 0)
        return 31;
    else if (strcmp(NEW_COLOUR, "green") == 0) 
        return 32;
    else if (strcmp(NEW_COLOUR, "orange") == 0)
        return 33;
    else if (strcmp(NEW_COLOUR, "blue") == 0)
        return 34;
    else if (strcmp(NEW_COLOUR, "purple") == 0)
        return 35;
    else if (strcmp(NEW_COLOUR, "cyan") == 0)
        return 36;
    else if (strcmp(NEW_COLOUR, "gray") == 0)
        return 37;
    else{
        shell_print(old_colour, "Unknown theme.\n");
        return old_colour;
    }
}


int main(){
    char input[255]; // for the user to input
    char *arguments[15]; // array for arguments
    const char delimeter[2] = " ";
    char command[255];
    EnvVar EnvVariables[255]; // enviroment variables array, currently set to 255
    int enviromentCounter = 0; // currently setting a global counter, we could readjust so that it initializes with NULL values and find the first null value to edit

    int shell_colour = 31; //default red theme colour value

    while(strcmp(input, "exit") != 10){ // For some reason strcmp is returning 10 if they are equal not 0???? 
        
        int counter = 0; // for indexing array

        shell_print(shell_colour, "cshell$ ");
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
            memmove(name, name+1, strlen(name)); // remove the $
            char *value = strtok(NULL, "=");
            EnvVar temp;
            temp.name = name;
            temp.value = value;
            EnvVariables[enviromentCounter++] = temp;
        }else if(strcmp(command, "print") == 0){
            char *token;
            for(int i = 1; i < counter; i++){
                token = arguments[i];
                if(token[0] == '$'){ // look through enviroment variables NEED TO ADD EDGE CASE FOR IF THE ENV VARIBALE DOES NOT EXIST / IF THERE ARE NO ENV VARIABLES
                    char *name = strtok(command, "="); // get just the name
                    memmove(name, name+1, strlen(name));
                    for(int j = 0; j < enviromentCounter; j++){ // for some reason only prints the most recent value
                        if(EnvVariables[j].name == name){
                            printf("%s \n", EnvVariables[j].value);
                        }
                    }
                }else{
                    printf("%s \n", arguments[i]);
                }
            }
        }else if(strcmp(command, "theme") == 0){
            shell_colour = change_theme(arguments[1], shell_colour);
        }
        else {
            printf("Current command is %s %d", command);
        }

    }
    shell_print(shell_colour, "goodbye");
    return 0;
}